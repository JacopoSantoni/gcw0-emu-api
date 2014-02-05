#ifndef _MENU_VIEW_H_
#define _MENU_VIEW_H_

#include <SDL.h>
#include "emu_interface.h"
#include "menu.h"
#include "view.h"

namespace gcw {

  class Manager;
  
  class MenuView : public View
  {
    private:
      Menu *menu;
    
    public:
      MenuView(Manager *manager);
    
      void setMenu(Menu *menu) { this->menu = menu; }
    
      virtual void render();
      virtual void handleEvents();
  };

  
}


#endif
