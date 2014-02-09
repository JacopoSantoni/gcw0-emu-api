#include "menu_view.h"

#include "menu.h"
#include "defines.h"
#include "manager.h"

#include <vector>

using namespace std;
using namespace gcw;

MenuView::MenuView(Manager *manager) : View(manager), current(nullptr)
{

}

void MenuView::handleEvents()
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
          case GCW_KEY_DOWN: down(); break;
          case GCW_KEY_UP: up(); break;
            
          case MENU_BACK_BUTTON: if (!menuStack.empty()) { current = menuStack.top(); menuStack.pop(); } break;
            
          case GCW_KEY_RIGHT:
          case GCW_KEY_LEFT:
          case MENU_ACTION_BUTTON:
            current.menu->entryAt(current.index)->action(manager, key); break;
            
          default: break;
        }
      }
    }
  }
}

void MenuView::down()
{
  if (current.index < current.menu->count()-1)
    ++current.index;
}

void MenuView::up()
{
  if (current.index > 0)
    --current.index;
}

void MenuView::enterSubmenu(SubMenuEntry *entry)
{
  menuStack.push(current);
  current = MenuStatus(entry->subMenu());
}


void MenuView::render()
{
  //gfx->blit(gfx->cache.get("data/systems/gba-small.png"), 100, 100);
  
  current.menu->render(gfx, 10, 10, 20, 40);  
  gfx->print(10,40+current.index*18, false, Font::bigFont, ">");
}