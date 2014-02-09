#ifndef _PATH_VIEW_H_
#define _PATH_VIEW_H_

#include <SDL.h>
#include "emu_interface.h"
#include "view.h"

#include <stack>

namespace gcw {

  class Manager;
  class Menu;
  class SubMenuEntry;
  
  class MenuView : public View
  {
    private:
      struct MenuStatus
      {
        Menu *menu;
        u8 index;
        
        MenuStatus(Menu *menu) : menu(menu), index(0) { }
      };
    
      std::stack<MenuStatus> menuStack;
      MenuStatus current;
      Menu *root;
    
      void down();
      void up();
    
    public:
      MenuView(Manager *manager);
    
      void setMenu(Menu *root) { this->root = root; current = MenuStatus(root); }
    
      virtual void render();
      virtual void handleEvents();
    
      void enterSubmenu(SubMenuEntry *entry);
  };

  
}


#endif
