#ifndef _MANAGER_H_
#define _MANAGER_H_

#include "loader.h"
#include "gfx.h"
#include "controls.h"
#include "utils.h"

namespace gcw {

class Manager
{
  private:
    Loader loader;
    Gfx gfx;
    Controls controls;
    Timer timer;
  
    bool running;
  
  public:
    Manager() : running(true) { }
    void scan() { loader.scan(); }
    void init();
    void run();
  
  
  
};

}

#endif