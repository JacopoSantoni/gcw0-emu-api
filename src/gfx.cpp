#include "gfx.h"

using namespace std;
using namespace gcw;


ImageCache Gfx::cache;
SDL_PixelFormat *Gfx::format;

void Gfx::init()
{
  SDL_Init(SDL_INIT_VIDEO|SDL_INIT_JOYSTICK);
	atexit(SDL_Quit);
  
  if(SDL_NumJoysticks() > 0)
    SDL_JoystickOpen(0);
  
  const int bpp = 16;
  
	//SDL_ShowCursor(SDL_DISABLE);
  screen = SDL_SetVideoMode(WIDTH, HEIGHT, bpp, SDL_HWSURFACE | SDL_DOUBLEBUF);
  
	if (screen->format->BitsPerPixel != bpp) {
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
  T *sp = reinterpret_cast<T*>(buffer.data);
  
  for (int i = 0; i < buffer.height; ++i)
    memcpy(p+(i*dest->w), sp+i*buffer.width, sizeof(T)*buffer.width);
  
  SDL_UnlockSurface(dest);
}

template <typename T>
void Gfx::clear(GfxBuffer &buffer, T color)
{
  T *p = reinterpret_cast<T*>(buffer.data);
  
  for (int w = 0; w < buffer.width; ++w)
    for (int h = 0; h < buffer.height; ++h)
      p[h*buffer.pitch + w] = color;
}


u16 Gfx::print(int x, int y, bool centered, const Font &font, const char *text)
{
  if (centered)
  {
    u16 length = font.stringWidth(text);
    x -= length/2;
  }
  
  u16 len = strlen(text);

  SDL_Rect rect;
  SDL_Rect out = rrr(x, y, 0, 0);
  
  for (u16 i = 0; i < len; ++i)
  {
    char c = text[i];
    
    if (c == '\n')
    {
      out.y += font.lineHeight;
      out.x = x;
    }
    else
    {
      u8 w = font.widths[static_cast<u8>(c)];
      rect = rrr(font.tileWidth * (c%32), font.tileHeight * (c/32), w, font.tileHeight);
      SDL_BlitSurface(font.image, &rect, screen, &out);
      out.x += w+1;
    }
  }
  
  return out.x;
}

u16 Gfx::printf(int x, int y, bool centered, const Font &font, const char *text, ...)
{
  char buffer[64];
  va_list args;
  va_start (args, text);
  vsnprintf (buffer, 64, text, args);
  va_end(args);
  
  return print(x,y,centered,font,buffer);
}

#pragma mark Font

Font::Font(string filename, u8 tileWidth, u8 tileHeight, u8 lineHeight, u8 defaultWidth, map<string,u8> customWidths) :
  image(IMG_Load(filename.c_str())),
  tileWidth(tileWidth),
  tileHeight(tileHeight),
  lineHeight(lineHeight)
{
  memset(widths, defaultWidth, 256);
  for (auto &pair : customWidths)
    for (char c : pair.first)
      widths[static_cast<u8>(c)] = pair.second;
}


const Font Font::bigFont = Font("data/font.png", 6, 9, 9, 5,
  {{"!.:|", 1},
   {"(),:`", 2},
   {" \"<=>[]ijl{}", 3},
   {"frt", 4}}
);


#pragma mark Template Instantiations

template void Gfx::clear(GfxBuffer &buffer, u32 color);
template void Gfx::clear(GfxBuffer &buffer, u16 color);

template void Gfx::rawBlit<u32>(SDL_Surface *dest, GfxBuffer &buffer, Offset &offset);
template void Gfx::rawBlit<u16>(SDL_Surface *dest, GfxBuffer &buffer, Offset &offset);

