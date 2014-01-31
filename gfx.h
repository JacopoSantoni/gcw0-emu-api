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

#define DEFAULT_FPS (60.0f)

namespace gcw
{
  class Gfx
  {
    private:
      struct FrameRate
      {
        std::chrono::steady_clock clock;
        std::chrono::microseconds ticksForFrame;
        
        u32 totalFrames;
        std::chrono::time_point<std::chrono::steady_clock> base;

        FrameRate() : ticksForFrame(static_cast<u32>(1000000 / DEFAULT_FPS)), totalFrames(0) { }
      };
    
      SDL_Surface *screen;
      SDL_PixelFormat *format;
    
      FrameRate frameRate;
      
    public:
      void init();
      inline void flip() { SDL_Flip(screen); }
    
      void setFrameRate(float rate);
      void frameRateDelay();
    
      u32 ccc(u8 r, u8 b, u8 g) { return SDL_MapRGB(format, r, g, b); }
  
      void clear(u32 color) {
        SDL_Rect rect = {0,0,static_cast<Uint16>(screen->w),static_cast<u16>(screen->h)};
        SDL_FillRect(screen, &rect, color);
      }

  };
}

#endif
