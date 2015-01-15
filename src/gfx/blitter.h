#ifndef __BLITTER_H__
#define __BLITTER_H__

#include "../common/defines.h"

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
  
class BlitterFactory
{
private:
  std::string name;
  
public:
  BlitterFactory(const std::string& name) : name(name) { }
  virtual Blitter* buildBlitter(GfxBufferFormat format) = 0;
  virtual void computeOffset(Offset& offset, u16 width, u16 height) = 0;
  const std::string& getName() { return name;}
  
  virtual ~BlitterFactory() { }
};
  
template<class B, u16 W, u16 H>
class BlitterFactorySimple : public BlitterFactory
{
public:
  BlitterFactorySimple(const std::string& name) : BlitterFactory(name) { }
  
  Blitter* buildBlitter(GfxBufferFormat format) override { return new B(); }
  void computeOffset(Offset& offset, u16 width, u16 height) override
  {
    offset.x = (width - W)/2;
    offset.y = (height - H)/2;
  }
};
  
  
template<GfxBufferFormat FROM, GfxBufferFormat TO>
class NativeBlitterFactory : public BlitterFactory
{
private:
  u16 sw, sh;
  
public:
  NativeBlitterFactory(u16 sw, u16 sh) : BlitterFactory("Native"), sw(sw), sh(sh) { }
  
  void computeOffset(Offset& offset, u16 width, u16 height) override
  {
    offset.x = (width - sw)/2;
    offset.y = (height - sh)/2;
  }
  
  Blitter* buildBlitter(GfxBufferFormat format) override
  {
    return new FormatBlitter<FROM, TO>();
  }
};
  
#	define likely(x)       __builtin_expect((x),1)
#	define unlikely(x)     __builtin_expect((x),0)
#	define prefetch(x, y)  __builtin_prefetch((x),(y))
  
  
  class GBFullBlit : public Blitter
  {
    inline void gb_upscale(uint32_t *to, uint32_t *from)
    {
      uint32_t reg1, reg2, reg3, reg4;
      unsigned int x,y;
      
      /* Before:
       *    a b
       *    c d
       *    e f
       *
       * After (parenthesis = average):
       *    a      a      b      b
       *    (a,c)  (a,c)  (b,d)  (b,d)
       *    c      c      d      d
       *    (c,e)  (c,e)  (d,f)  (d,f)
       *    e      e      f      f
       */
      
      for (y=0; y < 240/5; y++) {
        for(x=0; x < 320/4; x++) {
          prefetch(to+4, 1);
          
          /* Read b-a */
          reg2 = *from;
          reg1 = reg2 & 0xffff0000;
          reg1 |= reg1 >> 16;
          
          /* Write b-b */
          *(to+1) = reg1;
          reg2 = reg2 & 0xffff;
          reg2 |= reg2 << 16;
          
          /* Write a-a */
          *to = reg2;
          
          /* Read d-c */
          reg4 = *(from + 160/2);
          reg3 = reg4 & 0xffff0000;
          reg3 |= reg3 >> 16;
          
          /* Write d-d */
          *(to + 2*320/2 +1) = reg3;
          reg4 = reg4 & 0xffff;
          reg4 |= reg4 << 16;
          
          /* Write c-c */
          *(to + 2*320/2) = reg4;
          
          /* Write (b,d)-(b,d) */
          if (unlikely(reg1 != reg3))
            reg1 = ((reg1 & 0xf7def7de) >> 1) + ((reg3 & 0xf7def7de) >> 1);
          *(to + 320/2 +1) = reg1;
          
          /* Write (a,c)-(a,c) */
          if (unlikely(reg2 != reg4))
            reg2 = ((reg2 & 0xf7def7de) >> 1) + ((reg4 & 0xf7def7de) >> 1);
          *(to + 320/2) = reg2;
          
          /* Read f-e */
          reg2 = *(from++ + 2*160/2);
          reg1 = reg2 & 0xffff0000;
          reg1 |= reg1 >> 16;
          
          /* Write f-f */
          *(to + 4*320/2 +1) = reg1;
          reg2 = reg2 & 0xffff;
          reg2 |= reg2 << 16;
          
          /* Write e-e */
          *(to + 4*320/2) = reg2;
          
          /* Write (d,f)-(d,f) */
          if (unlikely(reg2 != reg4))
            reg2 = ((reg2 & 0xf7def7de) >> 1) + ((reg4 & 0xf7def7de) >> 1);
          *(to++ + 3*320/2) = reg2;
          
          /* Write (c,e)-(c,e) */
          if (unlikely(reg1 != reg3))
            reg1 = ((reg1 & 0xf7def7de) >> 1) + ((reg3 & 0xf7def7de) >> 1);
          *(to++ + 3*320/2) = reg1;
        }
        
        to += 4*320/2;
        from += 2*160/2;
      }
    }
    
    void blit(const GfxBuffer& buffer, const Offset& offset, SDL_Surface* dest)
    {
      gb_upscale(static_cast<u32*>(dest->pixels), buffer.getData<u32>());
    }
  };
  
  
}






#endif