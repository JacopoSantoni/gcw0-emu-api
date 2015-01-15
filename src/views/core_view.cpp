#include "core_view.h"

#include "../core/manager.h"

#include <vector>

using namespace std;
using namespace gcw;

#pragma mark Core Controls

void CoreView::initGfx()
{
  delete blitter;
  
  const GfxBufferSpec& gfxSpec = core->getGfxSpec();
  buffer.allocate(gfxSpec);
  core->setBuffer(buffer);
  offset.x = (WIDTH - buffer.width)/2;
  offset.y = (HEIGHT - buffer.height)/2;
  
  if (gfxSpec.format == FORMAT_RGB565)
    blitter = new FormatBlitter<FORMAT_RGB565, FORMAT_RGB565>();
  else if (gfxSpec.format == FORMAT_XRGB888)
  {
    if (manager->getGfx()->getFormat()->BitsPerPixel == 32)
      blitter = new FormatBlitter<FORMAT_XRGB888, FORMAT_RGBA8888>();
    else
      blitter = new FormatBlitter<FORMAT_XRGB888, FORMAT_RGB565>();
  }
}

void CoreView::initSfx()
{
  const optional<SfxAudioSpec>& sfxSpec = core->getSfxSpec();
  
  if (sfxSpec)
  {
    audioBuffer = unique_ptr<u32[]>(new u32[sfxSpec->bufferSize]);
    // TODO: should use sampleRate taken from core options
    audioOut = unique_ptr<AudioOut>(new AudioOut(sfxSpec->sampleRate, 133, 4, sfxSpec->bufferSize));
    core->setAudioBuffer(audioBuffer.get());
  }
}

void CoreView::reset()
{
  /* release gfx buffer */
  buffer.release();
  
  /* release sfx buffers if any */
  audioBuffer.reset();
  audioOut.reset();
  
  /* nullify core reference */
  core = nullptr;
}

void CoreView::initForCore(CoreInterface *core)
{
  reset();
  this->core = core;
  
  initControls();
  initGfx();
  initSfx();
}

void CoreView::initControls()
{
  const vector<ButtonSetting>& buttons = manager->getCurrentCoreInfo().supportedButtons();
  
  const ButtonStatus suspendKeys = manager->getSuspendShortcut();
  // persistence->loadCustomKeysForCore(core)
  // TODO: for each keybind saved replace buttons[i].key with the saved bind if override is enabled
  
  //reset button mapping
  for (int i = 0; i < GCW_KEY_COUNT+GCW_ANALOG_COUNT; ++i)
    mapping[i].enabled = false;

  analogMode = GCW_ANALOG_NONE;
  
  // for each available button of the emulator enable its definition and its shift amount
  for (const ButtonSetting &button : buttons)
  {
    // retrieve overridden controls if they are present
    optional<const Keybind&> bind = manager->getPersistence()->keyBindOveriddenFor(core->info().ident, button.getName());
    
    s8 index = -1;
    
    if (bind)
      index = Keys::indexForKey(bind->key);
    else if (button.isSingleKey())
      index = Keys::indexForKey(Keys::keyForMask(button.getMask()));
    // TODO: if !button.isSingleKey() we must manage it in a special way

    mapping[index].enabled = true;
    mapping[index].mask = 1 << button.getShiftAmount();

    if (index >= GCW_KEY_COUNT)
      analogMode = GCW_ANALOG_DIGITAL_MODE;
  }
  
  // each available button managed by the loader (not active on emulator) must be enabled anyway to check for events not directly
  // managed by the core. first we compute the whole button mask, so that we can find empty slots for unused required keys
  ButtonStatus usedMask = 0;
  suspendShortcut = 0;
  for (ButtonDefinition& def : mapping)
    if (def.enabled) usedMask |= def.mask;
    
  GCWKey keys[] = {GCW_KEY_L, GCW_KEY_R};
  for (GCWKey& key : keys)
  {
    s8 index = Keys::indexForKey(key);
    
    if (!mapping[index].enabled)
    {
      mapping[index].enabled = true;
      
      for (int i = 0; i < sizeof(ButtonStatus)*8; ++i)
        if ((usedMask & (1<<i)) == 0)
        {
          mapping[index].mask = 1 << i;
          usedMask |= 1 << i;
          
          if (suspendKeys & key)
            suspendShortcut |= 1 << i;
          
          break;
        }
    }
    else
    {
      suspendShortcut |= mapping[index].mask;
    }
  }
  
  // TODO: prepare real analog joypad if needed
  if (analogMode != GCW_ANALOG_DIGITAL_MODE)
  {
    if (core->isAnalogJoypadUsed())
    	analogMode = GCW_ANALOG_REAL_MODE;
  }
  
  if (analogMode != GCW_ANALOG_NONE)
  {
    // TODO: fetch global deadzone setting if present and override is enabled
    AnalogDeadZone deadZone = core->getAnalogDeadZone();
    
    analogDeadZone.min = deadZone.min*GCW_ANALOG_MAX;
    analogDeadZone.max = deadZone.max*GCW_ANALOG_MAX;
    analogDeadZone.delta = analogDeadZone.max - analogDeadZone.min;
    
    SDL_JoystickEventState(SDL_ENABLE);
  }
  
  // TODO: SDL_INIT_JOYSTICK should be enabled (always enable by on SDL_Init?)
  
  status = 0;
  
}

void CoreView::handleEvents()
{
  SDL_Event event;
  
  while (SDL_PollEvent(&event))
  {
    switch(event.type)
    {
      // manage normal keys
      case SDL_KEYDOWN:
      {
        // find the index in the mapping, or SDL_INVALID_KEY if nothing is found
        s8 index = Keys::indexForKey(static_cast<GCWKey>(event.key.keysym.sym));
        
        if (index != SDL_INVALID_KEY && index < GCW_KEY_COUNT && mapping[index].enabled)
          status |= mapping[index].mask;
        
        break;
      }
        
      case SDL_KEYUP:
      {
        s8 index = Keys::indexForKey(static_cast<GCWKey>(event.key.keysym.sym));
        
        if (index != SDL_INVALID_KEY && index < GCW_KEY_COUNT && mapping[index].enabled)
          status &=  ~mapping[index].mask;
        
        break;
      }
        
        
      // manage joypad
      case SDL_JOYAXISMOTION:
      {              
        // joypad is used for digital buttons
        if (analogMode == GCW_ANALOG_DIGITAL_MODE)
        {
          u8 axis = event.jaxis.axis;
          s16 value = event.jaxis.value;
          
          if (axis == GCW_ANALOG_AXIS_X)
          {
            if (value < -analogDeadZone.min && mapping[GCW_ANALOG_LEFT_INDEX].enabled)
              status |= mapping[GCW_ANALOG_LEFT_INDEX].mask;
            else if (value > analogDeadZone.min && mapping[GCW_ANALOG_RIGHT_INDEX].enabled)
              status |= mapping[GCW_ANALOG_RIGHT_INDEX].mask;
            else
              status &=  ~((mapping[GCW_ANALOG_LEFT_INDEX].mask) | mapping[GCW_ANALOG_RIGHT_INDEX].mask);
          }
          else if (axis == GCW_ANALOG_AXIS_Y)
          {
            if (value < -analogDeadZone.min && mapping[GCW_ANALOG_UP_INDEX].enabled)
              status |= mapping[GCW_ANALOG_UP_INDEX].mask;
            else if (value > analogDeadZone.min && mapping[GCW_ANALOG_DOWN_INDEX].enabled)
              status |= mapping[GCW_ANALOG_DOWN_INDEX].mask;
            else
              status &=  ~(mapping[GCW_ANALOG_UP_INDEX].mask | mapping[GCW_ANALOG_DOWN_INDEX].mask); // TODO: optimization - could be precomputed
          }

        }
        
        // joypad is used as a real analog, computes normalized values in 
        else if (analogMode == GCW_ANALOG_REAL_MODE)
        {
          u8 axis = event.jaxis.axis;
          s16 value = event.jaxis.value;
          
          if (axis == GCW_ANALOG_AXIS_X)
          {
            if (value < -analogDeadZone.min)
              analogStatus.x = value > -analogDeadZone.max ? static_cast<float>(value + analogDeadZone.min)/ analogDeadZone.delta : -1.0f;
            else if (value > analogDeadZone.min)
              analogStatus.x = value < analogDeadZone.max ? static_cast<float>(value - analogDeadZone.min)/ analogDeadZone.delta : 1.0f;
            else
              analogStatus.x = 0.0f;
          }
          else if (axis == GCW_ANALOG_AXIS_Y)
          {
            if (value < -analogDeadZone.min)
              analogStatus.y = value > -analogDeadZone.max ? static_cast<float>(value + analogDeadZone.min)/ analogDeadZone.delta : -1.0f;
            else if (value > analogDeadZone.min)
              analogStatus.y = value < analogDeadZone.max ? static_cast<float>(value - analogDeadZone.min)/ analogDeadZone.delta : 1.0f;
            else
              analogStatus.y = 0.0f;
          }
        }
      }
    }
  }
  
  //LOG("EXPECTED: %u - FOUND: %u\n",suspendShortcut,status);
  
  if (suspendShortcut == status)
  {
    manager->pauseEmulation();
    return;
  }
  
  core->setButtonStatus(status);
  core->setAnalogStatus(analogStatus);
}



void CoreView::render()
{
  core->emulationFrame();
  blitter->blit(buffer, offset, manager->getGfx()->screen);
}
