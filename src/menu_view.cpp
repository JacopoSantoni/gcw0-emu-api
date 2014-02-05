#include "menu_view.h"

#include "manager.h"

#include <vector>

using namespace std;
using namespace gcw;

MenuView::MenuView(Manager *manager) : View(manager)
{
  Menu *menu = new Menu();
  menu->addEntry(new MenuEntry("foobar"));
  menu->addEntry(new MenuEntry("antani"));
  menu->addEntry(new MenuEntry("sblinda"));
  menu->addEntry(new MenuEntry("ciccio"));
  this->menu = menu;
}

void MenuView::handleEvents()
{
  SDL_Event event;
  while (SDL_PollEvent(&event))
  {
    
  }
}


void MenuView::render()
{
  for (int i = 0; i < menu->count(); ++i)
  {
    manager->getGfx()->print(10, 40+i*10, false, Font::bigFont, menu->entryAt(i)->name().c_str());
  }
}