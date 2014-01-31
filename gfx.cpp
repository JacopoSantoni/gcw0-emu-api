#include "gfx.h"




using namespace std;
using namespace std::chrono;
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


void Gfx::setFrameRate(float rate)
{
  LOG("setting framerate to %f\n", rate);
  frameRate.totalFrames = 0;
  frameRate.ticksForFrame = microseconds(static_cast<u32>(1000000 / rate));
  frameRate.base = frameRate.clock.now();
}

void Gfx::frameRateDelay()
{
  frameRate.totalFrames++;
  
  time_point<steady_clock> current = frameRate.clock.now();
  time_point<steady_clock> target = frameRate.base + (frameRate.ticksForFrame * frameRate.totalFrames);
  
  if (current <= target)
  {
    microseconds delay = duration_cast<microseconds>(target-current);
    microseconds spent = duration_cast<microseconds>(current - (target - frameRate.ticksForFrame));
    microseconds total = delay + spent;
    
    LOG("delay: %lldus - spent: %lldus - total: %lldus\n", delay.count(), spent.count(), total.count());
    this_thread::sleep_for(target-current);
  }
  else
  {
    frameRate.base = current;
    frameRate.totalFrames = 0;
    // frame required more time than requested
  }
}

