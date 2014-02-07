#include "manager.h"

#include "menu.h"

using namespace gcw;

void Manager::init()
{
  loader.scan();
  collection.scan();
  
  core = loader.loadCore("dummy");
  
  gfx.init();
  timer.setFrameRate(60.0f);
  
  //currentView = &coreView;
  //coreView.initControls(core, GCW_KEY_L | GCW_KEY_R);
  //coreView.initGfx();
  
  MenuEntry *boolEntry = new BoolMenuEntry(new BoolSetting("Sound Enabled", "sound-enabled", true));
  
  StandardMenu *menu = new StandardMenu("Submenu");
  menu->addEntry(new StandardMenuEntry("foobar"));
  menu->addEntry(new StandardMenuEntry("antani"));
  menu->addEntry(new StandardMenuEntry("sblinda"));
  menu->addEntry(new StandardMenuEntry("ciccio"));
  menu->addEntry(new SubMenuEntry("Browse by System",new ConsoleMenu("Browse by System",collection.getConsoles())));
  
  StandardMenu *root = new StandardMenu("Root");
  root->addEntry(new StandardMenuEntry("asdella"));
  root->addEntry(new SubMenuEntry("submenu",menu));
  root->addEntry(boolEntry);
  menuView.setMenu(root);
  

  currentView = &menuView;
}

void Manager::run()
{
  
  while (running)
  {
    gfx.clear<u16>(gfx.ccc<u16>(0, 0, 0));
    
    currentView->handleEvents();
    currentView->render();

    
    
    //gfx.print(20, 20, false, Font::bigFont, "Browse ROMs by System");
    //gfx.print(20, 30, false, Font::bigFont, "Browse ROMs alphabetically");
    
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
