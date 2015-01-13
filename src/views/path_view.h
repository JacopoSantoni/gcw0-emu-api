#ifndef _MENU_VIEW_H_
#define _MENU_VIEW_H_

#include <SDL.h>
#include "../gfx/view.h"
#include "../gfx/ui.h"

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
        Path fakePath = Path();
        
      public:
        PathList(PathView* view) : OffsettableList(UI::LIST_SIZE), view(view) { }
        
        u32 current() const;
        u32 count() const;
        void set(u32 i);
        const Path& get(u32 i);
        
        const Path& selected() { return get(current()); }
        
        friend class PathView;
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
    
      void init(const std::string& title, const std::string& addThisCaption, const Path& path, std::function<void (const Path&)> lambda, std::function<void(void)> clambda);

      virtual void render();
      virtual void handleEvents();
  };

  
}


#endif
