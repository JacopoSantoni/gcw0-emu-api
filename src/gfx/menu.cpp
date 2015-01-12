#include "menu.h"

#include "../views/menu_view.h"
#include "../core/manager.h"

using namespace std;
using namespace gcw;

#pragma mark MenuEntry

void MenuEntry::render(Gfx *gfx, int x, int y)
{
  SDL_Surface* i = icon();
  
  if (i)
  {
    gfx->blitCentered(i, x+5, y);
    x += 5 + i->w;
  }
  
  if (isEnabled())
    gfx->print(x, y, false, Font::bigFont, name().c_str());
  else
    gfx->print(x, y, false, Font::bigFont, Gfx::ccc<u16>(160,160,160), name().c_str());

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

#pragma mark EnumMenuEntry

void EnumMenuEntry::action(Manager *manager, GCWKey key)
{
  if (key == GCW_KEY_RIGHT || key == MENU_ACTION_BUTTON)
    setting->next();
  else if (key == GCW_KEY_LEFT)
    setting->prev();
}

void EnumMenuEntry::render(Gfx *gfx, int x, int y)
{
  u16 width = gfx->print(x, y, false, Font::bigFont, name().c_str());
  gfx->print(x+width+10, y, false, Font::bigFont, setting->getValue()->getName().c_str());
  
}

#pragma mark PathSettingMenuEntry

void PathSettingMenuEntry::action(Manager *manager, GCWKey key)
{
  if (key == MENU_ACTION_BUTTON)
  {
    PathView* pview = manager->getPathView();
    
    auto lambda = [manager, this](const Path& path) {
      setting->setValue(path.value());
      manager->switchView(View::Type::MENU);
    };
    
    pview->init(pathViewTitle, Path(setting->getValue()), lambda, [manager, this](){ manager->switchView(View::Type::MENU); });
    manager->switchView(View::Type::PATH);
  }
}

void PathSettingMenuEntry::render(Gfx *gfx, int x, int y)
{
  u16 width = gfx->print(x, y, false, Font::bigFont, name().c_str());
  
  string &path = setting->getValue();
  gfx->print(x+width, y, false, Font::bigFont, Text::clipText(path, -30, "...").c_str());
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
  gfx->print(x, y, false, Font::bigFont, Text::clipText(path.value(), -40, "...").c_str());
}


#pragma mark SystemMenuEntry

SystemMenuEntry::SystemMenuEntry(const System::Spec& system, Menu *menu) : SubMenuEntry(system.name, menu), system(system), systemIcon(Gfx::cache.getFallback("data/consoles/"+system.ident+"-small.png","data/consoles/system-small.png"))
{
  
}

#pragma mark RomMenuEntry

void RomMenuEntry::render(Gfx *gfx, int x, int y)
{
  gfx->print(x, y, false, Font::bigFont, name().c_str());
}

void RomMenuEntry::action(Manager *manager, GCWKey key)
{
  if (manager->getCurrentRom() != &rom)
    manager->launchRom(rom);
  else
    manager->resumeEmulation();
}






void Menu::render(Gfx* gfx, int offset, int size, int tx, int ty, int x, int y, int c)
{
  gfx->print(tx, ty, false, Font::bigFont, title().c_str());

  for (int i = 0; i < size; ++i)
  {
    this->entryAt(i+offset)->render(gfx, x, y+spacing*i);
    
    if (c >= 0 && c == i + offset)
      gfx->print(x - UI::MENU_SELECTION_SPACING, y + spacing * i, false, Font::bigFont, ">");
  }
}


void RomPathsMenu::build()
{
  entries.clear();
  
  const vector<Path>& paths = persistence->getRomPaths();
  
  for (const Path &path : paths)
  {
    PathMenuEntry *entry = new PathMenuEntry(path);
    addEntry(entry);
  }

  auto lambda = [this](Manager* manager)
  {
    auto plambda = [manager, this](const Path& path)
    {
      persistence->addRomPath(path);
      build();
      manager->switchView(View::Type::MENU);
    };
    
    PathView* pview = manager->getPathView();
    pview->init("Add rom path", Path(Persistence::pathFor(PathType::HOME)), plambda, [manager, this](){ manager->switchView(View::Type::MENU); });
    manager->switchView(View::Type::PATH);
  };
  
  addEntry(new LambdaMenuEntry("Add Path", lambda));
}