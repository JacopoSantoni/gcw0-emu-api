#include "manager.h"

using namespace gcw;

void Manager::init()
{
  loader.scan();
  
  loader.loadCore("dummy");
  ((gcw::CoreControlsHandler*)controls.current())->initControls(loader.getCore());
  
  gfx.init();
  timer.setFrameRate(10.0f);

}

void Manager::run()
{
  while (running)
  {
    gfx.clear(gfx.ccc(rand()%256, rand()%256, rand()%256));

    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
      switch (event.type) {
        case SDL_QUIT: running = false; break;
          
        case SDL_KEYDOWN: running = false;
      }
    }
    
    gfx.flip();
    timer.frameRateDelay();
  }
}