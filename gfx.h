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

struct GfxBuffer
{
  u8 *base;
  u16 pitch;
  u16 width;
  u16 height;
  
  GfxBuffer() : base(nullptr), pitch(0), width(0), height(0) { }
  //void allocate(u16 width, u16 height, u16 pitch)
};

struct Offset
{
  u16 x;
  u16 y;
};

namespace gcw
{
  class Gfx
  {
    private:
    
      SDL_Surface *screen;
      static SDL_PixelFormat *format;
    
      template<typename T>
      void rawBlit(SDL_Surface *dest, GfxBuffer &buffer, Offset &offset);
    
    public:
      void init();
      inline void flip() { SDL_Flip(screen); }
    

    
      static u32 ccc(u8 r, u8 b, u8 g) { return SDL_MapRGB(format, r, g, b); }
  
      void clear(u32 color) {
        SDL_Rect rect = {0,0,static_cast<Uint16>(screen->w),static_cast<u16>(screen->h)};
        SDL_FillRect(screen, &rect, color);
      }
    
      void rawBlit(GfxBuffer &buffer, Offset &offset) { Gfx::rawBlit<u32>(screen, buffer, offset); }

    
      template<typename T>
      static void clear(GfxBuffer &buffer, T color);
  };
}

#endif
