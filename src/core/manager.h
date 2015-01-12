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
#include "../views/pause_view.h"

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
    PauseView pauseView;
    
    View *currentView;
    

  
    bool running;

  public:
    Manager() : core(nullptr), loader(this), persistence(this), collection(this), gfx(Gfx()),
    coreView(this),
    menuView(this),
    pathView(this),
    loadingView(this),
    pauseView(this),
    currentView(nullptr), running(true) { }
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
  
    void switchView(View::Type type)
    {
      currentView->deactivated();
      
      switch (type)
      {
        case View::Type::MENU: currentView = &menuView; break;
        case View::Type::CORE: currentView = &coreView; break;
        case View::Type::PATH: currentView = &pathView; break;
        case View::Type::LOADING: currentView = &loadingView; break;
        case View::Type::PAUSE: currentView = &pauseView; break;
      }
      
      currentView->activated();
    }
    
    bool isCoreLoaded() const { return core != nullptr; }
  
    void exit();
    void unloadCore();
    
    const RomEntry* getCurrentRom() { return rom; }
    
    void loadCoreAndWarmUp(CoreHandle& handle);
    void launchRom(const RomEntry& entry);
    void launchRom(const RomEntry& entry, CoreHandle& handle);
    
    void pauseEmulation();
    void resumeEmulation();
        
    ButtonStatus getSuspendShortcut() { return GCW_KEY_L | GCW_KEY_R; }
  };

}

#endif