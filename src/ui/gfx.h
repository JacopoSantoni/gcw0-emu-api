#ifndef _GFX_H_
#define _GFX_H_

#include "../common/defines.h"
#include "../common/utils.h"

#include <vector>
#include <map>
#include <string>
#include <chrono>
#include <thread>
#include <unordered_map>

#include <SDL.h>
#include <SDL_image.h>

#define WIDTH (320)
#define HEIGHT (240)

enum GfxBufferFormat
{
  FORMAT_8888,
  FORMAT_565
};

struct GfxBufferSpec
{
  u16 width;
  u16 height;
  GfxBufferFormat format;
};

struct GfxBuffer
{
  u8 *data;
  u16 pitch;
  u16 width;
  u16 height;
  
  GfxBuffer() : data(nullptr), pitch(0), width(0), height(0) { }
  //void allocate(u16 width, u16 height, u16 pitch)
  
  void allocate(const GfxBufferSpec& spec)
  {
    if (data) delete [] data;
    
    u32 bufferSize;
    pitch = spec.width;
    width = spec.width;
    height = spec.height;
    
    switch (spec.format)
    {
      case FORMAT_8888: bufferSize = width*height*sizeof(u32); break;
      case FORMAT_565: bufferSize = width*height*sizeof(u16); break;
    }
    
    data = new u8[bufferSize];

  }
  
  //~GfxBuffer() { if (data) delete [] data; }
};

struct Offset
{
  u16 x;
  u16 y;
};

namespace gcw
{
  struct Font;
  
  class ImageCache
  {
    private:
      std::unordered_map<std::string, SDL_Surface*> cache;
      
      SDL_Surface* load(std::string name)
      {
        SDL_Surface *image = IMG_Load(name.c_str());
        return image;
      }
      
    public:
      ImageCache() { }
    
      SDL_Surface *getFallback(const std::string& name, std::string fallback)
      {
        SDL_Surface *icon = get(name);
        
        if (!icon)
          return get(fallback);
        else
          return icon;
      }
    
      SDL_Surface *get(std::string name) {
        std::unordered_map<std::string, SDL_Surface*>::iterator it = cache.find(name);
        if (it != cache.end())
          return it->second;
        else
        {
          SDL_Surface *image = load(name);
          cache[name] = image;
          return image;
        }
      }
      
      ~ImageCache()
      {
        for (auto &pair : cache)
          SDL_FreeSurface(pair.second);
      }
  };
  
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
    
      template<typename T>
      static inline T ccc(u8 r, u8 b, u8 g) { return static_cast<T>(SDL_MapRGB(format, r, g, b)); }
  
      template<typename T>
      void clear(T color) {
        SDL_Rect rect = {0,0,static_cast<Uint16>(screen->w),static_cast<u16>(screen->h)};
        SDL_FillRect(screen, &rect, color);
      }
    
      void rawBlit(GfxBuffer &buffer, Offset &offset) { Gfx::rawBlit<u16>(screen, buffer, offset); }
    
      u16 print(int x, int y, bool centered, const Font &font, const char *text) const;
      u16 printf(int x, int y, bool centered, const Font &font, const char *text, ...) const;
    
      void blit(SDL_Surface *src, int x, int y)
      {
        SDL_Rect srcRect = rrr(0,0,src->w,src->h);
        SDL_Rect dstRect = rrr(x,y,0,0);
        
        SDL_BlitSurface(src, &srcRect, screen, &dstRect);
      }
    
      void blitCentered(SDL_Surface *src, int x, int y)
      {
        blit(src, x-src->w/2, y-src->h/2);
      }
    
      template<typename T>
      static void clear(GfxBuffer &buffer, T color);
    
      static ImageCache cache;
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
