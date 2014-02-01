#ifndef _GFX_H_
#define _GFX_H_

#include "defines.h"

#include <vector>
#include <map>
#include <string>
#include <chrono>
#include <thread>

#include <SDL.h>
#include <SDL_Image.h>

#define WIDTH (320)
#define HEIGHT (240)

enum GfxBufferFormat
{
  FORMAT_32BPP
};

struct GfxBufferSpec
{
  u16 width;
  u16 height;
  GfxBufferFormat format;
};

struct GfxBuffer
{
  u8 *base;
  u16 pitch;
  u16 width;
  u16 height;
  
  GfxBuffer() : base(nullptr), pitch(0), width(0), height(0) { }
  //void allocate(u16 width, u16 height, u16 pitch)
  
  void allocate(GfxBufferSpec spec)
  {
    if (base) delete [] base;
    
    u32 bufferSize;
    pitch = spec.width;
    width = spec.width;
    height = spec.height;
    
    switch (spec.format)
    {
      case FORMAT_32BPP: bufferSize = width*height*sizeof(u32); break;
    }
    
    base = new u8[bufferSize];

  }
};

struct Offset
{
  u16 x;
  u16 y;
};

namespace gcw
{
  struct Font;
  
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
    

      static inline SDL_Rect rrr(s16 x, s16 y, u16 w, u16 h) { return {x,y,w,h}; }
      static inline u32 ccc(u8 r, u8 b, u8 g) { return SDL_MapRGB(format, r, g, b); }
  
      void clear(u32 color) {
        SDL_Rect rect = {0,0,static_cast<Uint16>(screen->w),static_cast<u16>(screen->h)};
        SDL_FillRect(screen, &rect, color);
      }
    
      void rawBlit(GfxBuffer &buffer, Offset &offset) { Gfx::rawBlit<u32>(screen, buffer, offset); }
    
      void print(int x, int y, bool centered, const Font &font, const char *text);
      void printf(int x, int y, bool centered, const Font &font, const char *text, ...);

    
      template<typename T>
      static void clear(GfxBuffer &buffer, T color);
  };
  
  struct Font
  {
    protected:
      SDL_Surface *image;
      u8 widths[256];
      u8 tileWidth;
      u8 tileHeight;
      u8 lineHeight;
    
    public:
      Font(std::string filename, u8 defaultWidth, u8 tileWidth, u8 tileHeight, u8 lineHeight, std::map<std::string,u8> customWidths);
    
      u16 stringWidth(const char *text) const
      {
        u16 length = strlen(text), width = 0;
        for (int i = 0; i < length; ++i) width += widths[static_cast<u8>(text[i])];
        return width;
      }
    
      friend class Gfx;
    
      static const Font bigFont;
  };
}

#endif
