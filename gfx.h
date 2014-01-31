#ifndef _GFX_H_
#define _GFX_H_

#include "defines.h"

#include <vector>
#include <string>
#include <chrono>
#include <thread>

#include <SDL.h>

#define WIDTH (320)
#define HEIGHT (240)

namespace gcw
{
  class Gfx
  {
    private:
    
      SDL_Surface *screen;
      SDL_PixelFormat *format;
          
    public:
      void init();
      inline void flip() { SDL_Flip(screen); }
    

    
      u32 ccc(u8 r, u8 b, u8 g) { return SDL_MapRGB(format, r, g, b); }
  
      void clear(u32 color) {
        SDL_Rect rect = {0,0,static_cast<Uint16>(screen->w),static_cast<u16>(screen->h)};
        SDL_FillRect(screen, &rect, color);
      }

  };
}

#endif
