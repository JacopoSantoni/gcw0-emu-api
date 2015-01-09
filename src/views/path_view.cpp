#include "path_view.h"

#include "../common/utils.h"
#include "../gfx/gfx.h"

using namespace std;
using namespace gcw;

u32 PathView::PathList::current() const { return view->index; }
u32 PathView::PathList::count() const { return static_cast<u32>(view->folders.size()); }
void PathView::PathList::set(u32 i) { view->index = i; }
const Path& PathView::PathList::selected() { return view->folders[view->index]; }
const Path& PathView::PathList::get(u32 i) { return view->folders[offset+view->index]; }

PathView::PathView(Manager* manager) : View(manager), index(0), list(this) { }

void PathView::init(string title, const Path& path, function<void (const Path&)> lambda, function<void(void)> clambda)
{
  this->title = title;
  this->path = path;
  this->folders = path.subfolders();
  this->lambda = lambda;
  this->clambda = clambda;
}

void PathView::render()
{
  gfx->print(View::TITLE_OFFSET.x, View::TITLE_OFFSET.y, false, Font::bigFont, title.c_str());
  
  string path = this->path.value();
  gfx->print(View::TITLE_OFFSET.x, View::TITLE_OFFSET.y+10, false, Font::bigFont, Text::clipText(path, -40, "...").c_str());
  
  u32 count = list.getDisplayedAmount();
  for (int i = 0; i < count; ++i)
  {
    Path &folder = folders[i+list.minOffset()];
    
    gfx->print(View::MENU_OFFSET.x, View::MENU_OFFSET.y+14*i, false, Font::bigFont, Text::clipText(folder.value(), 30).c_str());
  }
  
  gfx->print(View::MENU_OFFSET.x-10,View::MENU_OFFSET.y+list.relativeIndex(list.current())*14, false, Font::bigFont, ">");
  
  //gfx->print(View::HELP_OFFSET.x,View::HELP_OFFSET.y, false, Font::bigFont, (string(Text::nameForKey(GCW_KEY_START)) + ": save changes").c_str());
}

void PathView::handleEvents()
{
  SDL_Event event;
  while (SDL_PollEvent(&event))
  {
    switch(event.type)
    {
        
        // manage normal keys
      case SDL_KEYDOWN:
      {
        GCWKey key = static_cast<GCWKey>(event.key.keysym.sym);
        
        switch (key)
        {
          case GCW_KEY_DOWN: list.down(); break;
          case GCW_KEY_UP: list.up(); break;
            
          case GCW_KEY_L: list.prevPage(); break;
          case GCW_KEY_R: list.nextPage(); break;
            
          case GCW_KEY_SELECT:
            // on SELECT cancel operation
            clambda();
            break;
            
          case GCW_KEY_START:
            // on START accept the current path
            lambda(path);
            break;
            
          //case GCW_KEY_RIGHT:
          //case GCW_KEY_LEFT:
          case MENU_BACK_BUTTON:
          case MENU_ACTION_BUTTON:
            if (list.selected() == ".." && !path.isRoot())
            {
              path = path.removeLast();
              folders = path.subfolders();
              list.reset();
              
              if (list.count() <= list.current())
                list.set(0);
            }
            else
            {
              path = path.append(list.selected());
              folders = path.subfolders();
              list.reset();
              
              if (list.count() <= list.current())
                list.set(0);
            }
            
            break;
            
          default: break;
        }
      }
    }
  }
}
