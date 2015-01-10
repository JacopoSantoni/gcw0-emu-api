#ifndef _MANAGER_H_
#define _MANAGER_H_

#include "loader.h"
#include "../gfx/gfx.h"
#include "../sfx/sfx.h"
#include "../common/utils.h"
#include "../data/rom_collection.h"
#include "../data/persistence.h"

#include "../views/core_view.h"
#include "../views/menu_view.h"
#include "../views/path_view.h"
#include "../views/loading_view.h"

namespace gcw {
  
  class Manager : public ManagerInterface
  {
  private:
    CoreInterface *core;
    const RomEntry *rom;
    
    Loader loader;
    Persistence persistence;
    RomCollection collection;
    Gfx gfx;
    Timer timer;
  
    CoreView coreView;
    MenuView menuView;
    PathView pathView;
    LoadingView loadingView;
    
    View *currentView;
    

  
    bool running;

  public:
    Manager() : core(nullptr), loader(this), collection(this), gfx(Gfx()), coreView(this), menuView(this), pathView(this), loadingView(this), currentView(nullptr), running(true) { }
    void scan() { loader.scan(); }
    void init();
    void run();
  
    MenuView *getMenuView() { return &menuView; }
    PathView *getPathView() { return &pathView; }
  
    Loader *getLoader() { return &loader; }
    Persistence *getPersistence() { return &persistence; }
    RomCollection *getRomCollection() { return &collection; }
    Gfx *getGfx() { return &gfx; }
    Timer *getTimer() { return &timer; }
    
    void reportRomLoading(float percent) override { }
    void reportRomLoaded() override { }
    
    const AudioStatus& writeAudioSamples(size_t count, size_t shift) override { return coreView.writeAudioSamples(count, shift); }
  
    void switchView(ViewType type)
    {
      switch (type)
      {
        case VIEW_MENU: currentView = &menuView; break;
        case VIEW_CORE: currentView = &coreView; break;
        case VIEW_PATH: currentView = &pathView; break;
        case VIEW_LOADING: currentView = &loadingView; break;
      }
    }
  
    void exit() {
      persistence.save();
      running = false;
    }
    
    const RomEntry* getCurrentRom() { return rom; }
    
    void loadCoreAndWarmUp(CoreHandle& handle);
    void launchRom(const RomEntry& entry);
    void launchRom(const RomEntry& entry, CoreHandle& handle);
    
    void pauseEmulation();
    void resumeEmulation();
  };

}

#endif