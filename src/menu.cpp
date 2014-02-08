#include "menu.h"

#include "menu_view.h"
#include "manager.h"

using namespace std;
using namespace gcw;

#pragma mark MenuEntry

void MenuEntry::render(Gfx *gfx, int x, int y)
{
  gfx->print(x, y, false, Font::bigFont, name().c_str());
}

#pragma mark SubMenuEntry

void SubMenuEntry::action(Manager *manager, GCWKey key)
{
  if (key == GCW_KEY_RIGHT)
    manager->getMenuView()->enterSubmenu(this);
}

#pragma mark BoolMenuEntry


void BoolMenuEntry::action(Manager *manager, GCWKey key)
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

ConsoleMenuEntry::ConsoleMenuEntry(ConsoleSpec *console) : console(console), icon(Gfx::cache.getFallback("data/consoles/"+console->ident+"-small.png","data/consoles/console-small.png"))
{
  
}

void ConsoleMenuEntry::render(Gfx *gfx, int x, int y)
{
  gfx->blitCentered(icon, x+5, y);
  gfx->print(x+5+icon->w, y, false, Font::bigFont, console->name.c_str());
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