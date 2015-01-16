#ifndef _MANAGER_H_
#define _MANAGER_H_

#include "loader.h"
#include "../gfx/gfx.h"
#include "../sfx/sfx.h"
#include "../common/utils.h"
#include "../common/optional.h"
#include "../data/rom_collection.h"
#include "../data/persistence.h"

#include "../views/core_view.h"
#include "../views/menu_view.h"
#include "../views/path_view.h"
#include "../views/loading_view.h"
#include "../views/pause_view.h"
#include "../views/keybind_view.h"
#include "../views/dialog_view.h"
#include "../views/help.h"

#include <stack>

namespace gcw {
  
  class Manager : public ManagerInterface
  {
  private:
    std::optional<std::reference_wrapper<CoreHandle>> handle;
    CoreInterface *core;
    const RomEntry *rom;
    
    Loader loader;
    Persistence persistence;
    RomCollection collection;
    Gfx gfx;
    Timer timer;
    
    Menus menus;
    Help help;
  
    CoreView coreView;
    MenuView menuView;
    PathView pathView;
    LoadingView loadingView;
    PauseView pauseView;
    KeybindView keybindView;
    DialogView dialogView;
    
    std::stack<View*> viewStack;
    
    View *currentView;
    
    bool running;

    bool emulating;

  public:
    Manager() : handle(std::nullopt), core(nullptr), loader(this), persistence(this), collection(this), gfx(Gfx()),
    help(this),
    coreView(this),
    menuView(this, menus.mainMenu),
    pathView(this),
    loadingView(this),
    pauseView(this, menus.pauseMenu),
    keybindView(this),
    dialogView(this),
    currentView(nullptr),
    running(true), emulating(false) { }
    
    void scan() { loader.scan(); }
    void init();
    void run();
    
    Menus* getMenus() { return &menus; }
    
    CoreView* getCoreView() { return &coreView; }
    MenuView* getMenuView() { return &menuView; }
    PathView* getPathView() { return &pathView; }
    KeybindView* getKeybindView() { return &keybindView; }
    DialogView* getDialogView() { return &dialogView; }
    template<typename T> T* getView() { return static_cast<T*>(currentView); }
    Help* getHelp() { return &help; }
  
    Loader *getLoader() { return &loader; }
    Persistence *getPersistence() { return &persistence; }
    RomCollection *getRomCollection() { return &collection; }
    Gfx *getGfx() { return &gfx; }
    Timer *getTimer() { return &timer; }
    
    bool isEmulating() const { return emulating; }
    
    const RomEntry* getCurrentRom() { return rom; }
    const CoreInterface* getCurrentCore() { return core; }
    const CoreInfo& getCurrentCoreInfo() { return (*handle).get().info; }
    
    void reportRomLoading(float percent) override { }
    void reportRomLoaded() override { }
    
    const AudioStatus& writeAudioSamples(size_t count, size_t shift) override { return coreView.writeAudioSamples(count, shift); }
    
    void forceSwitch(View::Type type)
    {
      while (!viewStack.empty()) viewStack.pop();
      switchView(type);
    }
  
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
        case View::Type::KEYBIND: currentView = &keybindView; break;
        case View::Type::DIALOG: currentView = &dialogView; break;
      }
      
      currentView->activated();
    }
    
    void pushView(View::Type type)
    {
      viewStack.push(currentView);
      switchView(type);
    }
    
    void popView()
    {
      currentView->deactivated();
      currentView = viewStack.top();
      viewStack.pop();
      currentView->activated();
    }
    
    bool isCoreLoaded() const { return core != nullptr; }
  
    void exit();
    void unloadCore();
    
    
    void loadCoreAndWarmUp(CoreHandle& handle);
    void launchRom(const RomEntry& entry);
    void launchRom(const RomEntry& entry, CoreHandle& handle);
    
    /* TODO: save sram first? */
    void softReset() { core->softReset(); }
    
    void pauseEmulation();
    void resumeEmulation();
    
    void stateSave(SaveSlot slot);
    void stateLoad(SaveSlot slot);
        
    ButtonStatus getSuspendShortcut() { return GCW_KEY_L | GCW_KEY_R; }
    
    
    static Manager* instance;
  };

}

#endif