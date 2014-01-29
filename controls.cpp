#include "controls.h"

#include <vector>

using namespace std;
using namespace gcw;

s8 Controls::indexForKey(GCWKey key)
{
  switch (key) {
    case GCWKEY_A: return 0;
    case GCWKEY_B: return 1;
    case GCWKEY_X: return 2;
    case GCWKEY_Y: return 3;
    case GCWKEY_LEFT: return 4;
    case GCWKEY_RIGHT: return 5;
    case GCWKEY_UP: return 6;
    case GCWKEY_DOWN: return 7;
    case GCWKEY_L: return 8;
    case GCWKEY_R: return 9;
    case GCWKEY_START: return 10;
    case GCWKEY_SELECT: return 11;
    default: return SDL_INVALID_KEY;
  }
}

void Controls::initControls(CoreInterface *core)
{
  vector<ButtonSetting> buttons = core->supportedButtons();
  
  // persistence->loadCustomKeysForCore(core)
  // for each keybind saved replace buttons[i].key with the saved bind
  
  //reset button mapping
  for (int i = 0; i < GCWKEY_AMOUNT; ++i)
    mapping[i].enabled = false;
  
  // for each available button of the emulator enable its definition and its shift amount
  for (ButtonSetting &button : buttons)
  {
    assert(button.key != )
    
    mapping[indexForKey(button.key)].enabled = true;
    mapping[indexForKey(button.key)].shiftAmount = button.shiftAmount;
  }
  
  status = 0;
}

void Controls::handleEvents()
{
  SDL_Event event;
  
  while (SDL_PollEvent(&event))
  {
    switch(event.type)
    {
      case SDL_KEYDOWN:
      {
        // find the index in the mapping, or SDL_INVALID_KEY if nothing is found
        s8 index = indexForKey(static_cast<GCWKey>(event.key.keysym.sym));
        
        if (index != SDL_INVALID_KEY && mapping[index].enabled)
          status |= 1 << mapping[index].shiftAmount;
        
        break;
      }
        
      case SDL_KEYUP:
      {
        s8 index = indexForKey(static_cast<GCWKey>(event.key.keysym.sym));
        
        if (index != SDL_INVALID_KEY && mapping[index].enabled)
          status &=  ~(1 << mapping[index].shiftAmount);
      }
    }
  }
}
