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
      class PathList : public OffsettableList<const std::string&>
      {
      private:
        PathView *view;
        
      public:
        PathList(PathView* view) : OffsettableList(12), view(view) { }
        
        u32 current() const;
        u32 count() const;
        void set(u32 i);
        const std::string& get(u32 i);
        
        const std::string& selected();
      };
    
      std::vector<std::string> folders;
      Path path;
      std::string title;
    
      std::function<void (const Path&)> lambda;
      std::function<void (void)> clambda;
    
      u32 index;
      PathList list;

    public:
      PathView(Manager *manager);
    
      void init(std::string title, const Path& path, std::function<void (const Path&)> lambda, std::function<void(void)> clambda);

      virtual void render();
      virtual void handleEvents();
  };

  
}


#endif
