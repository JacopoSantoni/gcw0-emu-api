#ifndef _MENU_VIEW_H_
#define _MENU_VIEW_H_

#include <SDL.h>
#include "../ui/view.h"
#include "../common/utils.h"

#include <vector>
#include <string>
#include <stack>
#include <functional>

namespace gcw {

  class Manager;
  class Menu;
  class SubMenuEntry;
  
  class PathView : public View
  {
    private:
      class PathList : public OffsettableList<const Path&>
      {
      private:
        PathView *view;
        
      public:
        PathList(PathView* view) : OffsettableList(10), view(view) { }
        
        u32 current() const;
        u32 count() const;
        void set(u32 i);
        const Path& get(u32 i);
        
        const Path& selected();
      };
    
      std::vector<Path> folders;
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
