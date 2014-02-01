#ifndef _MANAGER_H_
#define _MANAGER_H_

#include "loader.h"
#include "gfx.h"
#include "controls.h"
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
    Controls controls;
    Timer timer;
  
    bool running;
  
    GfxBuffer buffer;
  
  public:
    Manager() : core(nullptr), loader(this), collection(this), running(true) { }
    void scan() { loader.scan(); }
    void init();
    void run();
  
    Loader *getLoader() { return &loader; }
    RomCollection *getRomCollection() { return &collection; }
    Gfx *getGfx() { return &gfx; }
    Controls *getControls() { return &controls; }
    Timer *getTimer() { return &timer; }
  
  
  
};

}

#endif