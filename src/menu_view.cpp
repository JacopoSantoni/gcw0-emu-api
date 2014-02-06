#include "menu_view.h"

#include "menu.h"
#include "defines.h"
#include "manager.h"

#include <vector>

using namespace std;
using namespace gcw;

MenuView::MenuView(Manager *manager) : View(manager), current(nullptr)
{
  MenuEntry *boolEntry = new BoolMenuEntry(new BoolSetting("Sound Enabled", "sound-enabled", true));
  
  Menu *menu = new Menu("Submenu");
  menu->addEntry(new MenuEntry("foobar"));
  menu->addEntry(new MenuEntry("antani"));
  menu->addEntry(new MenuEntry("sblinda"));
  menu->addEntry(new MenuEntry("ciccio"));
  
  Menu *root = new Menu("Root");
  root->addEntry(new MenuEntry("asdella"));
  root->addEntry(new SubMenuEntry("submenu",menu));
  root->addEntry(boolEntry);
  
  current = MenuStatus(root);
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
        switch (event.key.keysym.sym)
        {
          case GCW_KEY_DOWN: down(); break;
          case GCW_KEY_UP: up(); break;
            
          case GCW_KEY_LEFT: if (!menuStack.empty()) { current = menuStack.top(); menuStack.pop(); } break;
            
          case GCW_KEY_RIGHT: current.menu->entryAt(current.index)->action(this, GCW_KEY_RIGHT); break;
            
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
  manager->getGfx()->print(10, 10, false, Font::bigFont, current.menu->title().c_str());

  
  for (int i = 0; i < current.menu->count(); ++i)
  {
    MenuEntry * entry = current.menu->entryAt(i);
    
    manager->getGfx()->print(20, 40+i*10, false, Font::bigFont, entry->name().c_str());
  }
  
  manager->getGfx()->print(10,40+current.index*10, false, Font::bigFont, ">");
}