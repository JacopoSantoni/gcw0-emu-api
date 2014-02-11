#ifndef _MANAGER_H_
#define _MANAGER_H_

#include "loader.h"
#include "gfx.h"
#include "utils.h"
#include "rom_collection.h"
#include "persistence.h"

#include "core_view.h"
#include "menu_view.h"
#include "path_view.h"

namespace gcw {
  
class Manager
{
  private:
    CoreInterface *core;
    Loader loader;
    Persistence persistence;
    RomCollection collection;
    Gfx gfx;
    Timer timer;
  
    CoreView coreView;
    MenuView menuView;
    PathView pathView;
    View *currentView;
  
    bool running;

  public:
    Manager() : core(nullptr), loader(this), collection(this), gfx(Gfx()), coreView(this), menuView(this), pathView(this), currentView(nullptr), running(true) { }
    void scan() { loader.scan(); }
    void init();
    void run();
  
    MenuView *getMenuView() { return &menuView; }
  
    Loader *getLoader() { return &loader; }
    Persistence *getPersistence() { return &persistence; }
    RomCollection *getRomCollection() { return &collection; }
    Gfx *getGfx() { return &gfx; }
    Timer *getTimer() { return &timer; }
  
    void exit() { running = false; }
  
  
  
};

}

#endif