#include "menu.h"

#include "menu_view.h"

using namespace std;
using namespace gcw;

#pragma mark MenuEntry

void MenuEntry::render(Gfx *gfx, int x, int y)
{
  gfx->print(x, y, false, Font::bigFont, name().c_str());
}

#pragma mark SubMenuEntry

void SubMenuEntry::action(MenuView *view, GCWKey key)
{
  if (key == GCW_KEY_RIGHT)
    view->enterSubmenu(this);
}

#pragma mark BoolMenuEntry


void BoolMenuEntry::action(MenuView *view, GCWKey key)
{
  if (key == GCW_KEY_RIGHT || key == GCW_KEY_LEFT)
  {
    bool newValue = !setting->getValue();
    setting->setValue(newValue);
  }
}

void BoolMenuEntry::render(Gfx *gfx, int x, int y)
{
  u16 width = gfx->print(x, y, false, Font::bigFont, name().c_str());
  gfx->print(x+width+10, y, false, Font::bigFont, setting->getValue()?"Yes":"No");

}

#pragma mark ConsoleMenuEntry

ConsoleMenuEntry::ConsoleMenuEntry(ConsoleSpec *console) : console(console), icon(Gfx::cache.get("data/consoles/"+console->ident+"-small.png"))
{
  
}

void ConsoleMenuEntry::render(Gfx *gfx, int x, int y)
{
  gfx->blit(icon, x, y);
  gfx->print(x+icon->w+3, y, false, Font::bigFont, console->ident.c_str());
}






ConsoleMenu::ConsoleMenu(string caption, vector<ConsoleSpec*>* consoles) : Menu("Browse by System"), consoles(consoles)
{
  
}



void Menu::render(Gfx* gfx, int tx, int ty, int x, int y)
{
  gfx->print(tx, ty, false, Font::bigFont, title().c_str());

  size_t count = this->count();
  for (int i = 0; i < count; ++i)
  {
    this->entryAt(i)->render(gfx, x, y+18*i);
  }
}