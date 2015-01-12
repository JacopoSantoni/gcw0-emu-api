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
  
    constexpr u16 LIST_SIZE = 10;

    constexpr u16 MENU_SELECTION_SPACING = 10;
    constexpr u16 MENU_DEFAULT_SPACING = 18;
    
    static inline void disableKeyRepeat() { SDL_EnableKeyRepeat(0, 0); }
    static inline void enableKeyRepeat() { SDL_EnableKeyRepeat(300, 80); }
  }
}

#endif