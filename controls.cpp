#include "controls.h"

#include <vector>

using namespace std;
using namespace gcw;

s8 Controls::indexForKey(GCWKey key)
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
    default: return SDL_INVALID_KEY;
  }
}

void Controls::initControls(CoreInterface *core)
{
  vector<ButtonSetting> buttons = core->supportedButtons();
  
  // persistence->loadCustomKeysForCore(core)
  // TODO: for each keybind saved replace buttons[i].key with the saved bind if override is enabled
  
  //reset button mapping
  for (int i = 0; i < GCW_KEY_COUNT; ++i)
    mapping[i].enabled = false;
  for (int i = 0; i < GCW_ANALOG_COUNT; ++i)
    analogMapping[i].enabled = false;
  
  analogMode = GCW_ANALOG_NONE;
  
  // for each available button of the emulator enable its definition and its shift amount
  for (ButtonSetting &button : buttons)
  {
    if (!button.isAnalogKey)
    {
      mapping[indexForKey(button.key)].enabled = true;
      mapping[indexForKey(button.key)].mask = 1 << button.shiftAmount;
    }
    else
    {
      analogMapping[button.analogKey].enabled = true;
      analogMapping[button.analogKey].mask = 1 << button.shiftAmount;
      analogMode = GCW_ANALOG_DIGITAL_MODE;
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
  }
  
  // TODO: SDL_INIT_JOYSTICK should be enabled (always enable by on SDL Init?)
  
  
  status = 0;
}

void Controls::handleEvents()
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
        
        if (index != SDL_INVALID_KEY && mapping[index].enabled)
          status |= mapping[index].mask;
        
        break;
      }
        
      case SDL_KEYUP:
      {
        s8 index = indexForKey(static_cast<GCWKey>(event.key.keysym.sym));
        
        if (index != SDL_INVALID_KEY && mapping[index].enabled)
          status &=  ~mapping[index].mask;
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
            if (value < -analogDeadZone.min)
              status |= analogMapping[GCW_ANALOG_LEFT].mask;
            else if (value > analogDeadZone.min)
              status |= analogMapping[GCW_ANALOG_RIGHT].mask;
            else
              status &=  ~((analogMapping[GCW_ANALOG_LEFT].mask) | analogMapping[GCW_ANALOG_RIGHT].mask);
          }
          else if (axis == GCW_ANALOG_AXIS_Y)
          {
            if (value < -analogDeadZone.min)
              status |= analogMapping[GCW_ANALOG_UP].mask;
            else if (value > analogDeadZone.min)
              status |= analogMapping[GCW_ANALOG_DOWN].mask;
            else
              status &=  ~(analogMapping[GCW_ANALOG_UP].mask | analogMapping[GCW_ANALOG_DOWN].mask); // TODO: optimization - could be precomputed
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
}
