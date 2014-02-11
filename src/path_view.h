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
      class PathList : public OffsettableList<std::string&>
      {
      private:
        PathView *view;
        
      public:
        PathList(PathView* view) : OffsettableList(10), view(view) { }
        
        u32 current();
        u32 count();
        void set(u32 i);
        std::string& get(u32 i);
        
        std::string& selected();
      };
    
      std::vector<std::string> folders;
      Path* path;
      std::string title;
    
      u32 index;
      PathList list;

    public:
      PathView(Manager *manager);
    
      void init(std::string title, Path *path);

      virtual void render();
      virtual void handleEvents();
  };

  
}


#endif
