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
  if (key == MENU_ACTION_BUTTON)
    manager->getMenuView()->enterSubmenu(this);
}

#pragma mark BoolMenuEntry


void BoolMenuEntry::action(Manager *manager, GCWKey key)
{
  if (key == GCW_KEY_RIGHT || key == GCW_KEY_LEFT || key == MENU_ACTION_BUTTON)
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

#pragma mark PathMenuEntry

void PathMenuEntry::action(Manager *manager, GCWKey key)
{
  if (key == MENU_ACTION_BUTTON)
  {
    //
  }
}

void PathMenuEntry::render(Gfx *gfx, int x, int y)
{
  u16 width = gfx->print(x, y, false, Font::bigFont, name().c_str());
  
  string &path = setting->getValue();
  const int MAX_LENGTH = 30;
  
  if (path.length() < MAX_LENGTH)
    gfx->print(x+width, y, false, Font::bigFont, path.c_str());
  else
    gfx->print(x+width, y, false, Font::bigFont, (path.substr(0, MAX_LENGTH)+"...").c_str());
}


#pragma mark SystemMenuEntry

SystemMenuEntry::SystemMenuEntry(SystemSpec *system, Menu *menu) : SubMenuEntry(system->ident, menu), system(system), icon(Gfx::cache.getFallback("data/consoles/"+system->ident+"-small.png","data/consoles/system-small.png"))
{
  
}

void SystemMenuEntry::render(Gfx *gfx, int x, int y)
{
  gfx->blitCentered(icon, x+5, y);
  gfx->print(x+5+icon->w, y, false, Font::bigFont, system->name.c_str());
}

#pragma mark RomMenuEntry

void RomMenuEntry::render(Gfx *gfx, int x, int y)
{
  gfx->print(x, y, false, Font::bigFont, name().c_str());
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