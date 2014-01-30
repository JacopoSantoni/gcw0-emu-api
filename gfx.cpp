#include "gfx.h"

using namespace std;
using namespace gcw;


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
  
  format = screen->format;
}


void Gfx::setFrameRate(u32 rate)
{
  frameRate.totalFrames = 0;
  frameRate.rate = rate;
  frameRate.rateTicks = 1000.0f / rate;
}

void Gfx::frameRateDelay()
{
  frameRate.totalFrames++;
  
  u32 current = SDL_GetTicks();
  frameRate.currentTicks = current;
  
  u32 target = frameRate.baseTicks + static_cast<u32>(frameRate.totalFrames * frameRate.rateTicks);
  
  if (current <= target)
  {
    LOG("delay: %d\n", target-current);
    SDL_Delay(target - current);
  }
  else
  {
    frameRate.baseTicks = frameRate.currentTicks;
    frameRate.totalFrames = 0;
  }
}

