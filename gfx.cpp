#include "gfx.h"

using namespace gcw;

SDL_PixelFormat *Gfx::format;

void Gfx::init()
{
  SDL_Init(SDL_INIT_VIDEO);
	atexit(SDL_Quit);
  
	//SDL_ShowCursor(SDL_DISABLE);
  screen = SDL_SetVideoMode(WIDTH, HEIGHT, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);
  
	if (screen->format->BitsPerPixel != 32) {
    fprintf(stderr, "ERROR: Did not get 32 bpp, got %u bpp instead.\n",
            screen->format->BitsPerPixel);
    exit(1);
	}
	if (!(screen->flags & SDL_HWSURFACE)) {
		fprintf(stderr, "WARNING: Did not get a hardware surface.\n");
	}
	if (!(screen->flags & SDL_DOUBLEBUF)) {
		fprintf(stderr, "WARNING: Did not get double buffering.\n");
	}
  
  Gfx::format = screen->format;
}


template <typename T>
void Gfx::rawBlit(SDL_Surface *dest, GfxBuffer &buffer, Offset &offset)
{
  SDL_LockSurface(dest);
  T *p = static_cast<T*>(dest->pixels) + offset.x + offset.y*dest->w;
  
  for (int i = 0; i < buffer.height; ++i)
    memcpy(p+(i*dest->w), buffer.base+i*buffer.width, sizeof(T)*buffer.width);
  
  SDL_UnlockSurface(dest);
}

template <typename T>
void Gfx::clear(GfxBuffer &buffer, T color)
{
  T *p = reinterpret_cast<u32*>(buffer.base);
  
  for (int w = 0; w < buffer.width; ++w)
    for (int h = 0; h < buffer.height; ++h)
      p[h*buffer.pitch + w] = color;
}


template void Gfx::clear(GfxBuffer &buffer, u32 color);
template void Gfx::rawBlit<u32>(SDL_Surface *dest, GfxBuffer &buffer, Offset &offset);

