#include "manager.h"

using namespace gcw;

void Manager::init()
{
  loader.scan();
  
  core = loader.loadCore("dummy");
  ((gcw::CoreControlsHandler*)controls.current())->initControls(core, GCW_KEY_L | GCW_KEY_R);
  
  gfx.init();
  timer.setFrameRate(60.0f);

}

void Manager::run()
{
  GfxBufferSpec gfxSpec = core->getGfxSpec();
  buffer.allocate(gfxSpec);
  core->setBuffer(buffer);
  Offset offset;
  offset.x = (WIDTH - buffer.width)/2;
  offset.y = (HEIGHT - buffer.height)/2;
  
  controls.swithToCoreControls(core);
  
  while (running)
  {
    gfx.clear(gfx.ccc(0, 0, 0));
    
    controls.handleEvents();
    core->setButtonStatus(controls.getButtonStatus());
    core->setAnalogStatus(controls.getAnalogStatus());
    
    core->emulationFrame();
    gfx.rawBlit(buffer, offset);
    
    gfx.print(20, 20, false, Font::bigFont, "Browse ROMs by System");
    gfx.print(20, 30, false, Font::bigFont, "Browse ROMs alphabetically");
    
    /*SDL_Event event;
    while (SDL_PollEvent(&event))
    {
      switch (event.type) {
        case SDL_QUIT: running = false; break;
          
        case SDL_KEYDOWN: running = false;
      }
    }*/
    
    gfx.flip();
    timer.frameRateDelay();
  }
}
