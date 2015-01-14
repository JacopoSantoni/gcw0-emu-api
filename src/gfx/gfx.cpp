#include "gfx.h"

using namespace std;
using namespace gcw;


ImageCache Gfx::cache;
SDL_PixelFormat *Gfx::format;

#ifdef SDL_TRIPLEBUF
  #define BUFFERING_POLICY SDL_TRIPLEBUF
#else
  #define BUFFERING_POLICY SDL_DOUBLEBUF
#endif

void Gfx::init()
{
  SDL_Init(SDL_INIT_VIDEO|SDL_INIT_JOYSTICK|SDL_INIT_AUDIO);
	atexit(SDL_Quit);
  
  if(SDL_NumJoysticks() > 0)
    SDL_JoystickOpen(0);
  
  const int bpp = 16;
  
	//SDL_ShowCursor(SDL_DISABLE);
  screen = SDL_SetVideoMode(WIDTH, HEIGHT, bpp, SDL_HWSURFACE | BUFFERING_POLICY);
  
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
void Gfx::rawBlit(SDL_Surface *dest, const GfxBuffer &buffer, const Offset &offset)
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
      p[h*buffer.width + w] = color;
}

template <typename T>
void Gfx::line(u16 x1, u16 y1, u16 x2, u16 y2, T color)
{
  bool hor = y1 == y2;
  T* p = static_cast<T*>(screen->pixels);
  
  if (hor)
    for (u16 x = x1; x <= x2; ++x)
      p[y1*screen->w + x] = color;
  else
    for (u16 y = y1; y <= y2; ++y)
      p[y*screen->w + x1] = color;
}

template <typename T>
void Gfx::rect(u16 x, u16 y, u16 w, u16 h, T color)
{
  line(x, y, x, y+h, color);
  line(x, y, x+w, y, color);
  line(x+w, y, x+w, y+h, color);
  line(x, y+h, x+w, y+h, color);
}

template <typename T>
void Gfx::rectFill(s16 x1, s16 y1, u16 w, u16 h, T color)
{
  SDL_Rect rect = {x1,y1,w,h};
  SDL_FillRect(screen, &rect, color);
}

template <typename T>
u16 Gfx::print(int x, int y, bool centered, const Font &font, const T color, const string& text) const
{
  size_t firstNewline = text.find_first_of('\n');
  
  if (firstNewline != string::npos)
  {
    print(x, y + font.lineHeight, centered, font, color, text.substr(firstNewline+1, string::npos));
    return print(x, y, centered, font, color, text.substr(0, firstNewline));
  }
  
  y -= font.tileHeight/2;
  
  if (centered)
  {
    u16 length = font.stringWidth(text.c_str());
    x -= length/2;
  }
  
  u16 len = text.length();
  
  SDL_Rect rect;
  SDL_Rect out = rrr(x, y, 0, 0);
  
  SDL_LockSurface(font.image);
  SDL_LockSurface(screen);
  
  u32 reference = Gfx::ccc<u32>(255, 255, 255);
  
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
      const u8 w = font.widths[static_cast<u8>(c)];
      rect = rrr(font.tileWidth * (c%32), font.tileHeight * (c/32), w, font.tileHeight);
      
      u32* sp = static_cast<u32*>(font.image->pixels) + rect.x + rect.y*font.image->w;
      T* dp = static_cast<T*>(screen->pixels) + out.x + out.y*screen->w;

      for (int h = 0; h < rect.h; ++h)
        for (int w = 0; w < rect.w; ++w)
        {
          T sc = *(sp + w + h*font.image->w);
          if (sc == reference) *(dp + w + h*screen->w) = color;
        }
      
      out.x += w+1;
    }
  }
  
  SDL_UnlockSurface(font.image);
  SDL_UnlockSurface(screen);
  
  return out.x;
}



u16 Gfx::print(int x, int y, bool centered, const Font &font, const string& text) const
{
  size_t firstNewline = text.find_first_of('\n');
  
  if (firstNewline != string::npos)
  {
    print(x, y + font.lineHeight, centered, font, text.substr(firstNewline+1, string::npos));
    return print(x, y, centered, font, text.substr(0, firstNewline));
  }
  
  y -= font.tileHeight/2;
  
  if (centered)
  {
    u16 length = font.stringWidth(text.c_str());
    x -= length/2;
  }
  
  u16 len = text.length();

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

static char buffer[128];

u16 Gfx::printf(int x, int y, bool centered, const Font &font, const char *text, ...) const
{
  va_list args;
  va_start (args, text);
  vsnprintf (buffer, 128, text, args);
  va_end(args);
  
  return print(x,y,centered,font,string(buffer));
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
   {" \"<=>[]ijl{}\x15", 3},
   {"frt", 4}}
);


#pragma mark Template Instantiations

template void Gfx::clear(GfxBuffer &buffer, u32 color);
template void Gfx::clear(GfxBuffer &buffer, u16 color);

template u16 Gfx::print<u16>(int x, int y, bool centered, const Font &font, const u16 color, const string& text) const;
template u16 Gfx::print<u32>(int x, int y, bool centered, const Font &font, const u32 color, const string& text) const;

template void Gfx::rawBlit<u32>(SDL_Surface *dest, const GfxBuffer &buffer, const Offset &offset);
template void Gfx::rawBlit<u16>(SDL_Surface *dest, const GfxBuffer &buffer, const Offset &offset);

template void Gfx::rectFill<u16>(s16 x1, s16 y1, u16 w, u16 h, u16 color);
template void Gfx::rectFill<u32>(s16 x1, s16 y1, u16 w, u16 h, u32 color);

template void Gfx::rect<u16>(u16 x, u16 y, u16 w, u16 h, u16 color);
template void Gfx::rect<u32>(u16 x, u16 y, u16 w, u16 h, u32 color);

template void Gfx::line<u16>(u16 x1, u16 y1, u16 x2, u16 y2, u16 color);
template void Gfx::line<u32>(u16 x1, u16 y1, u16 x2, u16 y2, u32 color);