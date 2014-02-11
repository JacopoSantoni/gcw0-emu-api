#include "path_view.h"

#include "utils.h"
#include "gfx.h"

using namespace std;
using namespace gcw;

u32 PathView::PathList::current() { return view->index; }
u32 PathView::PathList::count() { return static_cast<u32>(view->folders.size()); }
void PathView::PathList::set(u32 i) { view->index = i; }
string& PathView::PathList::selected() { return view->folders[view->index]; }
string& PathView::PathList::get(u32 i) { return view->folders[offset+view->index]; }

PathView::PathView(Manager* manager) : View(manager), index(0), list(this) { }

void PathView::init(string title, Path *path)
{
  this->title = title;
  this->path = path;
  this->folders = path->subfolders();
}

void PathView::render()
{
  gfx->print(View::TITLE_OFFSET.x, View::TITLE_OFFSET.y, false, Font::bigFont, title.c_str());
  
  const int MAX_LENGTH = 30;
  string path = this->path->value();

  if (path.length() < MAX_LENGTH)
    gfx->print(View::TITLE_OFFSET.x, View::TITLE_OFFSET.y+10, false, Font::bigFont, path.c_str());
  else
    gfx->print(View::TITLE_OFFSET.x, View::TITLE_OFFSET.y+10, false, Font::bigFont, (path.substr(0, MAX_LENGTH)+"...").c_str());
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
        
      }
    }
  }
}