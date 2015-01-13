#ifndef __UI_H__
#define __UI_H__

#include "../common/defines.h"

namespace gcw
{
  struct ViewOffset
  {
    u16 x, y;
  };
  
  namespace UI
  {
    constexpr ViewOffset TITLE_OFFSET = {10,10};
    constexpr ViewOffset MENU_OFFSET = {20,40};
    constexpr ViewOffset HELP_OFFSET = {10,230};
    
    constexpr ViewOffset REBIND_KEY_INFO = {160,160};
    constexpr ViewOffset REBIND_KEY_CAPTION = {160, 80};
    constexpr ViewOffset REBIND_KEY_STATUS = {160, 120};
    constexpr u16 REBIND_TIMEOUT_MSECS = 5000;
  
    constexpr u16 LIST_SIZE = 10;

    constexpr u16 MENU_SELECTION_SPACING = 10;
    constexpr u16 MENU_DEFAULT_SPACING = 18;
    
    static inline void disableKeyRepeat() { SDL_EnableKeyRepeat(0, 0); }
    static inline void enableKeyRepeat() { SDL_EnableKeyRepeat(300, 80); }
  }
}

#endif