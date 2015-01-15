#ifndef __BLITTER_H__
#define __BLITTER_H__

#include "defines.h"

namespace gcw {
  
  
template<GfxBufferFormat FROM, GfxBufferFormat TO>
class FormatConvert
{
  template<typename U, typename std::enable_if<FROM == FORMAT_XRGB888 && TO == FORMAT_RGB565>::type = 0>
  static inline u16 convert(u32 c)
  {
    
  }
};
  
class Blitter
{
protected:
  
  template<typename T>
  void rawBlit(SDL_Surface *dest, const GfxBuffer &buffer, const Offset &offset)
  {
    SDL_LockSurface(dest);
    T *p = static_cast<T*>(dest->pixels) + offset.x + offset.y*dest->w;
    T *sp = reinterpret_cast<T*>(buffer.data);
    
    for (int i = 0; i < buffer.height; ++i)
      memcpy(p+(i*dest->w), sp+i*buffer.width, sizeof(T)*buffer.width);
    
    SDL_UnlockSurface(dest);
  }
  
public:
  Blitter() { }
  
  virtual void blit(const GfxBuffer& buffer, const Offset& offset, SDL_Surface* dest) = 0;
  virtual ~Blitter() { }
};
  
  
template<GfxBufferFormat FROM, GfxBufferFormat TO> struct fake_dependency : public std::false_type {};
  
template<GfxBufferFormat FROM, GfxBufferFormat TO>
class FormatBlitter
{
public:
  void blit(const GfxBuffer& buffer, const Offset& offset, SDL_Surface* dest)
  {
    static_assert(fake_dependency<FROM, TO>::value, "must use specialization");
  }
  
};
  
template<>
inline void FormatBlitter<FORMAT_XRGB888, FORMAT_RGB565>::blit(const GfxBuffer& buffer, const Offset& offset, SDL_Surface* dest)
{
}

class Blitter565to565 : public Blitter
{
public:
  Blitter565to565() : Blitter() { }
  
  void blit(const GfxBuffer& buffer, const Offset& offset, SDL_Surface* dest) override
  {
    rawBlit<u16>(dest, buffer, offset);
  }
};

class Blitter888to888 : public Blitter
{
public:
  Blitter888to888() : Blitter() { }
  
  void blit(const GfxBuffer& buffer, const Offset& offset, SDL_Surface* dest) override
  {
    rawBlit<u32>(dest, buffer, offset);
  }
};

class Blitter888to565 : public Blitter
{
public:
  Blitter888to565() : Blitter() { }
  
  void blit(const GfxBuffer& buffer, const Offset& offset, SDL_Surface* dest) override
  {
    SDL_LockSurface(dest);
    u16* d = reinterpret_cast<u16*>(dest->pixels) + offset.x + offset.y*dest->w;
    u32* s = reinterpret_cast<u32*>(buffer.data);
    
    for (int y = 0; y < buffer.height; ++y)
    {
      u16* bd = d + y*dest->w;
      u32* bs = s + y*buffer.width;
      
      for (int x = 0; x < buffer.width; ++x, ++bd, ++bs)
      {
        u32 sc = *bs;
        
        u8 r = (sc >> 16) / 8;
        u8 g = ((sc >> 8) & 0xFF) / 4;
        u8 b = ((sc) & 0xFF) / 8;
        
        *bd = (r << 11) | (g << 5) | b;
      }
    }
    
    SDL_UnlockSurface(dest);
    
  }
};

}



#endif