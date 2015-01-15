#include "menu.h"

#include "../views/menu_view.h"
#include "../core/manager.h"

using namespace std;
using namespace gcw;

#pragma mark MenuEntry

void MenuEntry::render(Gfx *gfx, int x, int y, bool isSelected)
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
    manager->getView<MenuView>()->enterSubmenu(this);
}

#pragma mark SettingMenuEntry

bool SettingMenuEntry::canBeModified() const
{
  return canBeModifiedAtRuntime() || !Manager::instance->isEmulating();
}

void SettingMenuEntry::render(Gfx *gfx, int x, int y, bool isSelected)
{
  u16 color = !canBeModified() ? Gfx::ccc<u16>(160, 160, 160) : Gfx::ccc<u16>(255, 255, 255);
  
  u16 width = gfx->print(x, y, false, Font::bigFont, color, name().c_str());
  u16 basePosition = x+width+totalSpacing;
  u16 baseTextPosition = basePosition+Font::bigFont.stringWidth("\x14")+valueTextWidth/2;
  
  if (!canBeModified())
  {
    gfx->print(baseTextPosition, y, true, Font::bigFont, color, getValueName());
    return;
  }

  gfx->print(baseTextPosition, y, true, Font::bigFont, getValueName());
  
  if (isSelected)
  {
    gfx->print(basePosition, y, false, Font::bigFont, "\x14");
    gfx->print(baseTextPosition+valueTextWidth/2, y, false, Font::bigFont, "\x15");
  }
}

void SettingMenuEntry::action(Manager *manager, GCWKey key)
{
  if (!canBeModified())
  {
    DialogView* view = Manager::instance->getDialogView();
    view->initMessage("This setting can't be changed\nwhile a core is running.", [](){ Manager::instance->popView(); } );
    Manager::instance->pushView(View::Type::DIALOG);
    return;
  }
}

#pragma mark BoolMenuEntry


void BoolMenuEntry::action(Manager *manager, GCWKey key)
{
  if (!canBeModified())
  {
    SettingMenuEntry::action(manager, key);
    return;
  }
  
  if ((key == GCW_KEY_RIGHT || key == GCW_KEY_LEFT || key == MENU_ACTION_BUTTON) && canBeModified())
  {
    bool newValue = !setting->getValue();
    setting->setValue(newValue);
  }
}

#pragma mark EnumMenuEntry

void EnumMenuEntry::action(Manager *manager, GCWKey key)
{
  if (!canBeModified())
  {
    SettingMenuEntry::action(manager, key);
    return;
  }
  
  if (key == GCW_KEY_RIGHT || key == MENU_ACTION_BUTTON)
    setting->next();
  else if (key == GCW_KEY_LEFT)
    setting->prev();
}

#pragma mark BlitterMenuEntry

BlitterMenuEntry::BlitterMenuEntry(vector<const BlitterFactory*>& blitters) : SettingMenuEntry("Scaler")
{
  u16 maxWidth = 0;
  for (const auto* blitter : blitters)
  {
    u16 width = Font::bigFont.stringWidth(blitter->getName().c_str());
    maxWidth = std::max(width, maxWidth);
    this->blitters.push_back(unique_ptr<const BlitterFactory>(blitter));
  }

  setValueTextWidth(maxWidth);
  current = this->blitters.begin();
}

const string BlitterMenuEntry::getValueName() const
{
  return (*current)->getName();
}

void BlitterMenuEntry::action(Manager *manager, GCWKey key)
{
  if (key == GCW_KEY_RIGHT || key == MENU_ACTION_BUTTON)
  {
    if (current != blitters.end()-1)
      ++current;
    else
      current = blitters.begin();
  }
  else if (key == GCW_KEY_LEFT)
  {
    if (current != blitters.begin()+1)
      --current;
    else
      current = blitters.end();
  }
  
  manager->getCoreView()->setBlitter(current->get());
  if (manager->isEmulating())
    manager->getCoreView()->updatedBlitter();
}

#pragma mark PathSettingMenuEntry

void PathSettingMenuEntry::action(Manager *manager, GCWKey key)
{
  if (!canBeModified())
  {
    SettingMenuEntry::action(manager, key);
    return;
  }
  
  if (key == MENU_ACTION_BUTTON)
  {
    PathView* pview = manager->getPathView();
    
    auto lambda = [manager, this](const Path& path) {
      setting->setValue(path.value());
      manager->popView();
    };
    
    pview->init(pathViewTitle, "Set this path", Path(setting->getValue()), lambda, [manager, this](){ manager->popView(); });
    manager->pushView(View::Type::PATH);
  }
}

void PathSettingMenuEntry::render(Gfx *gfx, int x, int y, bool isSelected)
{
  u16 color = (!canBeModified()) ? Gfx::ccc<u16>(160, 160, 160) : Gfx::ccc<u16>(255, 255, 255);
  u16 width = gfx->print(x, y, false, Font::bigFont, color, name().c_str());
  
  const string&  path = setting->getValue();
  gfx->print(x+width, y, false, Font::bigFont, color, Text::clipText(path, -30, "...").c_str());
}

#pragma mark PathMenuEntry

void PathMenuEntry::action(Manager *manager, GCWKey key)
{
  if (key == MENU_ACTION_BUTTON)
  {
    //
  }
}

void PathMenuEntry::render(Gfx *gfx, int x, int y, bool isSelected)
{
  gfx->print(x, y, false, Font::bigFont, Text::clipText(path.value(), -40, "...").c_str());
}


#pragma mark SystemMenuEntry

SystemMenuEntry::SystemMenuEntry(const System::Spec& system, Menu *menu) : SubMenuEntry(system.name, menu), system(system), systemIcon(Gfx::cache.getFallback("data/consoles/"+system.ident+"-small.png","data/consoles/system-small.png"))
{
  
}

#pragma mark RomMenuEntry

void RomMenuEntry::render(Gfx *gfx, int x, int y, bool isSelected)
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
    bool selected = c >= 0 && c == i + offset;
    
    this->entryAt(i+offset)->render(gfx, x, y+spacing*i, selected);
    
    if (selected)
      gfx->print(x - UI::MENU_SELECTION_SPACING, y + spacing * i, false, Font::bigFont, gfx->ccc<u16>(0, 255, 0),  "\x15");
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
      manager->popView();
    };
    
    PathView* pview = manager->getPathView();
    pview->init("Add rom path", "Add this path", Path(Persistence::pathFor(PathType::HOME)), plambda, [manager, this](){ manager->popView(); });
    manager->pushView(View::Type::PATH);
  };
  
  
  sort(entries.begin(), entries.end(), [](const unique_ptr<MenuEntry>& p1, const unique_ptr<MenuEntry>& p2){
    PathMenuEntry *e1 = static_cast<PathMenuEntry*>(p1.get());
    PathMenuEntry *e2 = static_cast<PathMenuEntry*>(p2.get());
    
    return e1->getPath() < e2->getPath();
  });
  
  addEntry(new LambdaMenuEntry("Add Path", lambda));

}

void CoreMenu::build(CoreHandle& handle)
{
  setTitle("Core "+handle.name());
  clear();
  
  StandardMenu *keybinds = new StandardMenu("Keys for "+handle.name());
  
  const auto& buttons = handle.info.supportedButtons();
  
  /* compute max button name length for correct alignment */
  u16 maxWidth = 0;
  for (const auto& button : buttons)
    maxWidth = std::max(maxWidth, Font::bigFont.stringWidth(button.getName().c_str()));
  
  for (const auto& button : buttons)
  {
    KeybindMenuEntry *keyentry = new KeybindMenuEntry(button, handle, maxWidth+10);
    keybinds->addEntry(keyentry);
  }
  
  static auto resetBindsLambda = [](const CoreHandle& handle, Manager* manager)
  {
    static auto resetBinds = [&handle, manager]() {
      for (const auto& button : handle.info.supportedButtons())
        button.setMask(button.getDefaultMask());
      
      manager->popView();
    };
    
    manager->getDialogView()->initConfirm(string("Are you sure you want to reset all binds\nfor ")+handle.name()+"?", resetBinds, [manager](){ manager->popView(); });
    manager->pushView(View::Type::DIALOG);
  };
  
  LambdaMenuEntry *resetBindsEntry = new LambdaMenuEntry("Reset to default", std::bind(resetBindsLambda, cref(handle), placeholders::_1));
  
  keybinds->addEntry(resetBindsEntry);
  
  StandardMenu* settings = new StandardMenu("Settings");
  
  StandardMenu* settingsVideo = new StandardMenu("Video Settings");
  settings->addEntry(new SubMenuEntry("Video Settings", settingsVideo));
  
  vector<const BlitterFactory*> blitters;
  const BlitterFactory* native = Manager::instance->getCoreView()->computeNativeBlitter(handle.info.gfxSpec);
  if (native)
   blitters.push_back(native);
  
  Manager::instance->getCoreView()->setBlitter(native);
  
  blitters.push_back(new BlitterFactorySimple<GBFullBlit, WIDTH, HEIGHT>("Fullscreen"));
  MenuEntry* scalersEntry = new BlitterMenuEntry(blitters);
  
  settingsVideo->addEntry(scalersEntry);
  
  addEntry(new SubMenuEntry("Keys", keybinds));
  addEntry(new SubMenuEntry("Settings", settings));
}


void CoreMenuEntry::action(Manager *manager, GCWKey key)
{
  if (key == MENU_ACTION_BUTTON)
    manager->getMenus()->getCoreMenu()->build(core);
  
  SubMenuEntry::action(manager, key);
}


void KeybindMenuEntry::action(Manager *manager, GCWKey key)
{
  if (key == MENU_ACTION_BUTTON && setting.isRebindable())
  {
    KeybindView* view = manager->getKeybindView();
    
    std::string text;
    
    if (core)
      text = "Binding key '"+setting.getName()+"' of core "+core->get().name();
    // TODO else
    
    if (!setting.canBeMultikey())
    {
      static auto lambda = [this,manager](const ButtonSetting& setting, GCWKey key)
      {
        setting.setMask(key);
        manager->popView();
      };

      view->init(std::bind(lambda, cref(setting), placeholders::_1), [manager](){ manager->popView(); }, text);
      manager->pushView(View::Type::KEYBIND);
    }
    else
    {
      static auto lambda = [this, manager](const ButtonSetting& setting, ButtonStatus status)
      {
        setting.setMask(status);
        manager->popView();
      };
      
      view->initm(std::bind(lambda, cref(setting), placeholders::_1), [manager](){ manager->popView(); }, text);
      manager->pushView(View::Type::KEYBIND);
    }
  }
}

void KeybindMenuEntry::render(Gfx* gfx, int x, int y, bool isSelected)
{
  if (isEnabled())
  {
    gfx->print(x, y, false, Font::bigFont, setting.getName());
    gfx->print(x+spacing, y, false, Font::bigFont, setting.mnemonic());

  }
  else
  {
    gfx->print(x, y, false, Font::bigFont, Gfx::ccc<u16>(200,200,200), setting.getName());
    gfx->print(x+spacing, y, false, Font::bigFont, Gfx::ccc<u16>(200,200,200), setting.mnemonic());
  }
}


void Menus::buildCoresMenu(Manager *manager)
{
  Menu* menu = getCoresMenu();
  
  menu->clear();
  
  auto& cores = manager->getLoader()->getCores();
  
  for (CoreHandle& core : cores)
    menu->addEntry(new CoreMenuEntry(core, manager->getMenus()->getCoreMenu()));
  
  static auto rescanCores = [manager](Manager* manager) {
    // TODO
  };
  
  menu->addEntry(new LambdaMenuEntry("Rescan for cores", rescanCores));
}

void Menus::buildSystemsMenu(const RomCollection *collection)
{
  Menu* menu = getSystemsMenu();
  
  menu->clear();
  
  const std::vector<System::Spec>& systems = System::getSystems();
  std::vector<System::Spec>::const_iterator it;
  
  for (it = systems.begin(); it != systems.end(); ++it)
  {
    const System::Spec& system = *it;
    RomIteratorRange roms = collection->getRomsForSystem(system);
    
    if (roms.first != roms.second)
      menu->addEntry(new SystemMenuEntry(system, new RomsMenu(system.name, roms)));
  }
}
