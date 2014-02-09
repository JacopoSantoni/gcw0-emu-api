#ifndef _MENU_VIEW_H_
#define _MENU_VIEW_H_

#include <SDL.h>
#include "view.h"

#include <stack>

namespace gcw {

  class Manager;
  class Menu;
  class SubMenuEntry;
  
  class PathView : public View
  {
    private:

    public:
      PathView(Manager *manager);

      virtual void render();
      virtual void handleEvents();
  };

  
}


#endif
