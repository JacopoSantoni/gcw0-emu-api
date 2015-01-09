#ifndef _PATH_VIEW_H_
#define _PATH_VIEW_H_

#include <SDL.h>
#include "../core/emu_interface.h"
#include "../ui/view.h"

#include <stack>

namespace gcw {

  class Manager;
  class Menu;
  class SubMenuEntry;
  class MenuEntry;
  
  class MenuView : public View
  {
    private:
      struct MenuStatus
      {
        Menu *menu;
        u8 index;
        
        MenuStatus() : menu(nullptr), index(0) { }
        MenuStatus(Menu *menu) : menu(menu), index(0) { }
      };
    
      class MenuEntryList : public OffsettableList<MenuEntry*>
      {
        private:
          MenuStatus currentStatus;
          
        public:
          MenuEntryList() : OffsettableList(10) { }
        
          u32 current() const;
          u32 count() const;
          void set(u32 i);
          MenuEntry* get(u32 i);
        
          MenuEntry* selected();
        
          void setStatus(MenuStatus status) { currentStatus = status; }
          MenuStatus status() { return currentStatus; }
      };
    
      MenuEntryList list;
      std::stack<MenuStatus> menuStack;
    
    public:
      MenuView(Manager *manager);
    
      void setMenu(Menu *root) { /*this->root = root;*/ list.setStatus(MenuStatus(root)); }
    
      virtual void render();
      virtual void handleEvents();
    
      void enterSubmenu(SubMenuEntry *entry);
  };

  
}


#endif
