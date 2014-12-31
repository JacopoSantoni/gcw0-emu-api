#include "manager.h"

#include "menu.h"

using namespace gcw;

void Manager::init()
{
  /*Path path = Path("/Users/jack/Documents/Dev/github/gcw0-emu-api/");
  
  std::vector<std::string> folders = path.subfolders();
  for (std::string &f : folders)
    std::cout << f << std::endl;*/
  
  persistence.addRomPath(Persistence::ROOT_PATH+"media/SD/gba/");
  persistence.addRomPath(Persistence::ROOT_PATH+"media/SD/snes/");
  
  loader.scan();
  collection.scan();
  
  core = loader.loadCore("dummy");
  
  gfx.init();
  timer.setFrameRate(60.0f);
  
  //currentView = &coreView;
  //coreView.initControls(core, GCW_KEY_L | GCW_KEY_R);
  //coreView.initGfx();
  
  EnumSet sampleRates = {
    new ConcreteEnumValue<int32_t>("0",0),
    new ConcreteEnumValue<int32_t>("11025",11025),
    new ConcreteEnumValue<int32_t>("22050",22050),
    new ConcreteEnumValue<int32_t>("44100",44100),
    new ConcreteEnumValue<int32_t>("48000",48000)
  };

  StandardMenu *root = new StandardMenu("Root");
  root->addEntry(new SubMenuEntry("Browse by System",new SystemsMenu("Browse by System",&collection)));
  root->addEntry(new BoolMenuEntry(new BoolSetting("Sound Enabled", "sound-enabled", true)));
  root->addEntry(new EnumMenuEntry(new EnumSetting("Sample Rate", "sample-rate", sampleRates, sampleRates[3])));
  root->addEntry(new PathMenuEntry(new PathSetting("Saves path", "save-path", "/Users/jack/Documents/Dev/github/gcw0-emu-api/xcode/snes")));
  root->addEntry(new LambdaMenuEntry("Exit",[](Manager *manager){ manager->exit(); }) );
  menuView.setMenu(root);
  

  pathView.init("Select path", &persistence.getRomPaths()[0]);
  
  //currentView = &pathView;
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
