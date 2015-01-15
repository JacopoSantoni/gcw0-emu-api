#ifndef __BLITTER_H__
#define __BLITTER_H__

#include "defines.h"

namespace gcw {
  
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
  
  
template<GfxBufferFormat FROM, GfxBufferFormat TO>
class FormatBlitter : public Blitter
{
private:
  typedef typename std::conditional<FROM == FORMAT_XRGB888 || FROM == FORMAT_RGBA8888, u32, u16>::type pixel_from;
  typedef typename std::conditional<TO == FORMAT_XRGB888 || TO == FORMAT_RGBA8888, u32, u16>::type pixel_to;
  
public:
  void blit(const GfxBuffer& buffer, const Offset& offset, SDL_Surface* dest)
  {
    SDL_LockSurface(dest);
    pixel_to* d = reinterpret_cast<pixel_to*>(dest->pixels) + offset.x + offset.y*dest->w;
    pixel_from* s = reinterpret_cast<pixel_from*>(buffer.data);
    
    for (int y = 0; y < buffer.height; ++y)
    {
      pixel_to* bd = d + y*dest->w;
      pixel_from* bs = s + y*buffer.width;
      
      for (int x = 0; x < buffer.width; ++x, ++bd, ++bs)
        *bd = convert(*bs);
    }
    
    SDL_UnlockSurface(dest);
  }
  
  template< bool cond, typename U > using conversion_type  = typename std::enable_if<cond, U >::type;
  
  template<GfxBufferFormat F = FROM>
  static inline conversion_type<F == FORMAT_XRGB888 && TO == FORMAT_RGB565, pixel_to> convert(pixel_from c)
  {
    u8 r = (c >> 16) / 8;
    u8 g = ((c >> 8) & 0xFF) / 4;
    u8 b = ((c) & 0xFF) / 8;
    
    return (r << 11) | (g << 5) | b;
  }
  
  template<GfxBufferFormat F = FROM>
  static inline conversion_type<F == FORMAT_XRGB888 && TO == FORMAT_RGBA8888, pixel_to> convert(pixel_from c)
  {
    return c << 8;
  }
};
  

  
template<>
inline void FormatBlitter<FORMAT_RGB565, FORMAT_RGB565>::blit(const GfxBuffer &buffer, const Offset &offset, SDL_Surface *dest)
{
  rawBlit<u16>(dest, buffer, offset);
}
  
template<>
inline void FormatBlitter<FORMAT_RGBA5551, FORMAT_RGBA5551>::blit(const GfxBuffer &buffer, const Offset &offset, SDL_Surface *dest)
{
  rawBlit<u16>(dest, buffer, offset);
}
  
template<>
inline void FormatBlitter<FORMAT_RGBA8888, FORMAT_RGBA8888>::blit(const GfxBuffer &buffer, const Offset &offset, SDL_Surface *dest)
{
  rawBlit<u32>(dest, buffer, offset);
}
  
}



#endif