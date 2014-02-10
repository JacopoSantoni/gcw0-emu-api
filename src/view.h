#ifndef _VIEW_H_
#define _VIEW_H_

#include "defines.h"

namespace gcw
{
  struct ViewOffset
  {
    const u16 x, y;
    
    ViewOffset(const u16 x, const u16 y) : x(x), y(y) { }
  };
  
  
  class Manager;
  class Gfx;

  class View
  {
    protected:
      Manager* const manager;
      Gfx* const gfx;
    
    public:
      View(Manager *manager);
      virtual void render() = 0;
      virtual void handleEvents() = 0;
    
    
    static const ViewOffset TITLE_OFFSET;
    static const ViewOffset MENU_OFFSET;
  };
  
}


#endif