#include "manager.h"

using namespace gcw;

void Manager::init()
{
  loader.scan();
  
  loader.loadCore("dummy");
  ((gcw::CoreControlsHandler*)controls.current())->initControls(loader.getCore());
  
  gfx.init();
  timer.setFrameRate(60.0f);

}

void Manager::run()
{
  buffer.base = reinterpret_cast<u8*>(new u32[160*144*sizeof(u32)]);
  buffer.width = 160;
  buffer.pitch = 160;
  buffer.height = 144;
  loader.getCore()->setBuffer(buffer);
  Offset offset;
  offset.x = (WIDTH - 160)/2;
  offset.y = (HEIGHT - 144)/2;
  
  while (running)
  {
    gfx.clear(gfx.ccc(0, 0, 0));
    
    loader.getCore()->emulationFrame();
    gfx.rawBlit(buffer, offset);
    
    gfx.print(20, 20, false, Font::bigFont, "antani");
    
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