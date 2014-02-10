#ifndef _MENU_VIEW_H_
#define _MENU_VIEW_H_

#include <SDL.h>
#include "view.h"
#include "utils.h"

#include <vector>
#include <string>
#include <stack>

namespace gcw {

  class Manager;
  class Menu;
  class SubMenuEntry;
  
  class PathView : public View
  {
    private:
      std::vector<std::string> folders;
      Path* path;

    public:
      PathView(Manager *manager);
    
      void init(Path *path);

      virtual void render();
      virtual void handleEvents();
  };

  
}


#endif
