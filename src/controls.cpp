#include "controls.h"

#include "manager.h"

#include <vector>

using namespace std;
using namespace gcw;

s8 CoreControlsHandler::indexForKey(GCWKey key)
{
  switch (key) {
    case GCW_KEY_A: return 0;
    case GCW_KEY_B: return 1;
    case GCW_KEY_X: return 2;
    case GCW_KEY_Y: return 3;
    case GCW_KEY_LEFT: return 4;
    case GCW_KEY_RIGHT: return 5;
    case GCW_KEY_UP: return 6;
    case GCW_KEY_DOWN: return 7;
    case GCW_KEY_L: return 8;
    case GCW_KEY_R: return 9;
    case GCW_KEY_START: return 10;
    case GCW_KEY_SELECT: return 11;
      
    case GCW_ANALOG_LEFT: return 12;
    case GCW_ANALOG_RIGHT: return 13;
    case GCW_ANALOG_UP: return 14;
    case GCW_ANALOG_DOWN: return 15;
      
    default: return SDL_INVALID_KEY;
  }
}

void CoreControlsHandler::initControls(CoreInterface *core, ButtonStatus suspendKeys)
{
  vector<ButtonSetting> buttons = core->supportedButtons();
  
  // persistence->loadCustomKeysForCore(core)
  // TODO: for each keybind saved replace buttons[i].key with the saved bind if override is enabled
  
  //reset button mapping
  for (int i = 0; i < GCW_KEY_COUNT+GCW_ANALOG_COUNT; ++i)
    mapping[i].enabled = false;

  analogMode = GCW_ANALOG_NONE;
  
  // for each available button of the emulator enable its definition and its shift amount
  for (ButtonSetting &button : buttons)
  {
    s8 index = indexForKey(button.key);

    mapping[indexForKey(button.key)].enabled = true;
    mapping[indexForKey(button.key)].mask = 1 << button.shiftAmount;

    if (index >= GCW_KEY_COUNT)
      analogMode = GCW_ANALOG_DIGITAL_MODE;
  }
  
  // each available button managed by the combiner (not active on emulator) must be enabled anyway to check for events not directly
  // managed by the core. first we compute the whole button mask, so that we can find empty slots for unused required keys
  ButtonStatus usedMask = 0;
  suspendShortcut = 0;
  for (ButtonDefinition& def : mapping)
    if (def.enabled) usedMask |= def.mask;
    
  GCWKey keys[] = {GCW_KEY_L, GCW_KEY_R};
  for (GCWKey& key : keys)
  {
    if (!mapping[indexForKey(key)].enabled)
    {
      mapping[indexForKey(key)].enabled = true;
      
      for (int i = 0; i < sizeof(ButtonStatus)*8; ++i)
        if ((usedMask & (1<<i)) == 0)
        {
          mapping[indexForKey(key)].mask = 1 << i;
          usedMask |= 1 << i;
          
          if (suspendKeys & key)
            suspendShortcut |= 1 << i;
          
          break;
        }
    }
  }
  
  // TODO: prepare real analog joypad if needed
  if (analogMode != GCW_ANALOG_REAL_MODE)
  {
    // ..
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

void CoreControlsHandler::handleEvents()
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
        s8 index = indexForKey(static_cast<GCWKey>(event.key.keysym.sym));
        
        if (index != SDL_INVALID_KEY && index < GCW_KEY_COUNT && mapping[index].enabled)
          status |= mapping[index].mask;
        
        break;
      }
        
      case SDL_KEYUP:
      {
        s8 index = indexForKey(static_cast<GCWKey>(event.key.keysym.sym));
        
        if (index != SDL_INVALID_KEY && index < GCW_KEY_COUNT && mapping[index].enabled)
          status &=  ~mapping[index].mask;
        
        break;
      }
        
        
      // manage joypad
      case SDL_JOYAXISMOTION:
      {
                    LOG("V < DZ: %1.4f %1.4", event.jaxis.value,analogDeadZone.min);
        
        // joypad is used for digital buttons
        if (analogMode == GCW_ANALOG_DIGITAL_MODE)
        {
          u8 axis = event.jaxis.axis;
          s16 value = event.jaxis.value;
          
          if (axis == GCW_ANALOG_AXIS_X)
          {
            if (value < -analogDeadZone.min)
              status |= mapping[GCW_ANALOG_LEFT].mask;
            else if (value > analogDeadZone.min)
              status |= mapping[GCW_ANALOG_RIGHT].mask;
            else
              status &=  ~((mapping[GCW_ANALOG_LEFT].mask) | mapping[GCW_ANALOG_RIGHT].mask);
          }
          else if (axis == GCW_ANALOG_AXIS_Y)
          {
            if (value < -analogDeadZone.min)
              status |= mapping[GCW_ANALOG_UP].mask;
            else if (value > analogDeadZone.min)
              status |= mapping[GCW_ANALOG_DOWN].mask;
            else
              status &=  ~(mapping[GCW_ANALOG_UP].mask | mapping[GCW_ANALOG_DOWN].mask); // TODO: optimization - could be precomputed
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
    controls->manager->exit();
  }
  
}
