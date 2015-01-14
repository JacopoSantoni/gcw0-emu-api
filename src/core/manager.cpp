#include "manager.h"

#include "../gfx/menu.h"

using namespace std;
using namespace gcw;

void Manager::init()
{
  /*Path path = Path("/Users/jack/Documents/Dev/github/gcw0-emu-api/");
  
  std::vector<std::string> folders = path.subfolders();
  for (std::string &f : folders)
    std::cout << f << std::endl;*/
  
  /*persistence.addRomPath(Persistence::ROOT_PATH+"media/SD/gba/");
  persistence.addRomPath(Persistence::ROOT_PATH+"media/SD/snes/");
  persistence.addRomPath(Persistence::ROOT_PATH+"media/SD/gb/");*/

  persistence.createFolderStructure();
  persistence.load();
  
  loader.scan();
  collection.scan();
  
  gfx.init();
  timer.setFrameRate(60.0f);
  
  UI::enableKeyRepeat();
  
  //persistence.savePath(loader.getCores()[0].info.ident, &collection.getRomsForSystem(System::getSpecForSystem(System::Type::GAME_BOY_COLOR)).second->second, 0);
  
  EnumSet sampleRates = {
    new ConcreteEnumValue<int32_t>("0",0),
    new ConcreteEnumValue<int32_t>("11025",11025),
    new ConcreteEnumValue<int32_t>("22050",22050),
    new ConcreteEnumValue<int32_t>("44100",44100),
    new ConcreteEnumValue<int32_t>("48000",48000)
  };
  
  RomPathsMenu *romPathsMenu = new RomPathsMenu("Rom Paths", getPersistence());
  romPathsMenu->build();
  
  StandardMenu *root = new StandardMenu("Root");
  root->addEntry(new SubMenuEntry("Cores",new CoresMenu(this)));
  root->addEntry(new SubMenuEntry("Browse by System",new SystemsMenu("Browse by System",&collection)));
  root->addEntry(new SubMenuEntry("Configure Rom Paths",romPathsMenu));
  root->addEntry(new BoolMenuEntry(new BoolSetting("Sound Enabled", "sound-enabled", true)));
  root->addEntry(new EnumMenuEntry(new EnumSetting("Sample Rate", "sample-rate", sampleRates, sampleRates[3])));
  root->addEntry(new PathSettingMenuEntry(new PathSetting("Saves path", "save-path", "/Users/jack/Documents/Dev/github/gcw0-emu-api/xcode/root/usr/local/home/saves"), "Choose saves path"));
  root->addEntry(new LambdaMenuEntry("Exit",[](Manager* manager){ manager->exit(); }) );
  
  menus.setMainMenu(root);
  menuView.resetToRoot();
  
  currentView = &menuView;
  
  /*keybindView.initm([](ButtonStatus key){ cout << "mask " << key << endl; }, [](){ cout << "canceled" << endl; }, "Rebind key 'Start' of Gambatte core");
  currentView = &keybindView;
  currentView->activated();*/
  
  //currentView = &pauseView;
}

void Manager::run()
{  
  while (running)
  {
    gfx.clear<u16>(gfx.ccc<u16>(0, 0, 0));
    
    currentView->handleEvents();
    currentView->render();

    gfx.flip();
    timer.frameRateDelay();
  }
}

void Manager::loadCoreAndWarmUp(CoreHandle& handle)
{
  if (core)
  {
    /* if the loaded core is the same that is required we need to reset it */
    if (handle.isLoaded() && core->info() == handle.core->info())
    {
      core->reset();
      /* we reinit controls */
      coreView.initControls();
      coreView.flushAudioBuffer();
    }
    /* if manager already has a core but it is different from the one required we need to unload resources and unload core */
    else
    {
      unloadCore();
      
      core = loader.loadCore(handle);
      this->handle = optional<reference_wrapper<CoreHandle>>(ref(handle));
      core->initialize();
    }

  }
  /* otherwise load core and initialize it */
  else
  {
    core = loader.loadCore(handle);
    this->handle = optional<reference_wrapper<CoreHandle>>(ref(handle));
    coreView.initForCore(core);
    core->initialize();
  }
  }

void Manager::launchRom(const RomEntry& entry)
{
  const auto cores = loader.findCoresForSystem(entry.system.type);
  
  /* if no cores are found for the system we should prompt the user and let him choose a core in case */
  if (cores.size() == 0)
  {
    
  }
  /* if just a core is found there is no doubt */
  else if (cores.size() == 1)
  {
    launchRom(entry, cores[0]);
  }
  /* if more cores are found we should find if a default one is forced, otherwise let user choose one */
  else
  {
    optional<CoreIdentifier> identifier = persistence.defaultCoreForEntry(entry);
    
    // we found a core that has been forced on specified folder, we use it if it's still there
    if (identifier)
    {
      auto core = find_if(cores.begin(), cores.end(), [&](const reference_wrapper<CoreHandle>& handle) { return handle.get().info.ident == identifier; });
      
      if (core != cores.end())
        launchRom(entry, *core);
    }
  }
}

void Manager::launchRom(const RomEntry& entry, CoreHandle& handle)
{
  rom = &entry;
  
  loadCoreAndWarmUp(handle);
  
  /* TODO: manage multithreaded loading if required */
  if (!core->hasFeature(CoreFeature::REQUIRES_MULTI_THREADING_LOADING))
  {
    if (core->hasFeature(CoreFeature::CAN_SAVE_RAM))
    {
      Path sramPath = persistence.sramPath(core->info(), rom);
      
      core->sramSetPath(sramPath.value());
      core->loadRomByFileName(entry.path.value());
      core->sramLoadFrom(sramPath.value(), rom->name);
    }
    else
      core->loadRomByFileName(entry.path.value());

    pauseView.initialize(core, rom);
    menus.getCoreMenu()->build(handle);
    switchView(View::Type::CORE);
    
    if (handle.core->getSfxSpec())
      SDL_PauseAudio(0);
  }
  
}

void Manager::pauseEmulation()
{
  SDL_PauseAudio(1);
  core->emulationSuspended();
  switchView(View::Type::PAUSE);
}

void Manager::resumeEmulation()
{
  SDL_PauseAudio(0);
  core->emulationResumed();
  /* we reinit controls since a keybind could have changed */
  coreView.initControls();
  switchView(View::Type::CORE);
}

void Manager::stateSave(SaveSlot slot)
{
  Path path = persistence.savePath(core->info(), rom, slot);
  core->stateSaveTo(path.value());
}

void Manager::stateLoad(SaveSlot slot)
{
  Path path = persistence.savePath(core->info(), rom, slot);
  core->stateLoadFrom(path.value());
}


void Manager::unloadCore()
{
  /* reset core view buffers */
  coreView.reset();
  
  /* warn core that it is going to be unloaded */
  core->emulationStopped();
  core->releaseResources();
  
  /* ask the loader to unload the core (through dlclose) */
  loader.unload(core);
  core = nullptr;
  this->handle = nullopt;
}

void Manager::exit()
{
  /* if a core is loaded we must unload it before exiting */
  if (core)
    unloadCore();
  
  persistence.save();
  running = false;
}
