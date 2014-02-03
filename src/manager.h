#ifndef _MANAGER_H_
#define _MANAGER_H_

#include "loader.h"
#include "gfx.h"
#include "core_view.h"
#include "utils.h"
#include "rom_collection.h"

namespace gcw {
  
class Manager
{
  private:
    CoreInterface *core;
    Loader loader;
    RomCollection collection;
    Gfx gfx;
    Timer timer;
  
    CoreView coreView;
    View *currentView;
  
    bool running;
  
    GfxBuffer buffer;
  
    
  
  public:
    Manager() : core(nullptr), loader(this), collection(this), coreView(this), running(true) { }
    void scan() { loader.scan(); }
    void init();
    void run();
  
    Loader *getLoader() { return &loader; }
    RomCollection *getRomCollection() { return &collection; }
    Gfx *getGfx() { return &gfx; }
    Timer *getTimer() { return &timer; }
  
    void exit() { running = false; }
  
  
  
};

}

#endif