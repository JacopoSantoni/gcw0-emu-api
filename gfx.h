#ifndef _GFX_H_
#define _GFX_H_

#include "defines.h"

#include <vector>
#include <string>
#include <SDL.h>

#define WIDTH (320)
#define HEIGHT (240)

#define DEFAULT_FPS (60.0f)

namespace gcw
{
  class Gfx
  {
    private:
      struct FrameRate
      {
        u32 totalFrames;
        u32 baseTicks;
        u32 currentTicks;
        
        float rateTicks;
        u32 rate;
        
        FrameRate() : totalFrames(0), baseTicks(1), currentTicks(0), rateTicks(1000.0f / DEFAULT_FPS), rate(DEFAULT_FPS) { }
      };
    
      SDL_Surface *screen;
      SDL_PixelFormat *format;
    
      FrameRate frameRate;
    
      u32 ticks() { u32 ticks = SDL_GetTicks(); return ticks > 0 ? ticks : 1; }
  
    public:
      void init();
      inline void flip() { SDL_Flip(screen); }
    
      void setFrameRate(u32 rate);
      void frameRateDelay();
    
      u32 ccc(u8 r, u8 b, u8 g) { return SDL_MapRGB(format, r, g, b); }
  
      void clear(u32 color) {
        SDL_Rect rect = {0,0,static_cast<Uint16>(screen->w),static_cast<u16>(screen->h)};
        SDL_FillRect(screen, &rect, color);
      }

  };
}

#endif
