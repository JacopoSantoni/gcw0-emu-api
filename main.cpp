#include "emu_interface.h"

#include <SDL.h>

#include "loader.h"
#include "emu_interface.h"
#include "controls.h"

#include "dlfcn.h"

#include "gfx.h"


int run(int argc, char **argv)
{
  gcw::Loader loader;
  gcw::Controls controls;
  gcw::Gfx gfx;
  
  loader.scan();
  
  loader.loadCore("dummy");
  ((gcw::CoreControlsHandler*)controls.current())->initControls(loader.getCore());

  gfx.init();
  gfx.setFrameRate(59.73);
  
  bool running = true;
  
  while (running)
  {
    gfx.clear(gfx.ccc(rand()%256, rand()%256, rand()%256));
    
    gfx.frameRateDelay();
    
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
      switch (event.type) {
        case SDL_QUIT: running = false;
      }
    }
    
    gfx.flip();
  }

  /*if (argc > 1)
  {
    gcw::CoreHandle *core = loader.coreForFile(argv[1]);
    if (core)
    {
      loader.loadCore(core->info.ident);
      loader.getCore()->run(argc, argv);
    }
  }*/
  
  return 0;
}
