#ifndef _MENU_H_
#define _MENU_H_

#include "../common/defines.h"

#include "gfx.h"
#include "ui.h"
#include "../data/settings.h"
#include "../data/rom_collection.h"
#include "../data/persistence.h"
#include "../core/loader.h"

#include <string>
#include <vector>
#include <memory>

namespace gcw {
  
class Menu;
class MenuView;

class MenuEntry
{
  public:
    MenuEntry() { }
    virtual ~MenuEntry() { }
    virtual const std::string& name() = 0;
    virtual SDL_Surface* icon() { return nullptr; }
  
    virtual void action(Manager *manager, GCWKey key) { }
    virtual void render(Gfx* gfx, int x, int y);
};
  
class StandardMenuEntry : public MenuEntry
{
  protected:
    const std::string caption;
    
  public:
    StandardMenuEntry(std::string caption) : caption(caption) { }
    virtual const std::string& name() { return caption; }
    
    virtual void action(Manager *manager, GCWKey key) { }
};
  
class LambdaMenuEntry : public StandardMenuEntry
{
  protected:
    std::function<void (Manager*)> lambda;
  
  public:
    LambdaMenuEntry(std::string caption, std::function<void (Manager*)> lambda) : StandardMenuEntry(caption), lambda(lambda) { }

    virtual void action(Manager *manager, GCWKey key) { if (key == GCW_KEY_B) lambda(manager); }
};

class SubMenuEntry : public StandardMenuEntry
{
  protected:
    std::unique_ptr<Menu> menu;
 
  public:
    SubMenuEntry(std::string caption, Menu *menu) : StandardMenuEntry(caption+" >"), menu(menu) { }
  
    Menu *subMenu() { return menu.get(); }
  
    virtual void action(Manager *manager, GCWKey key);
};
  
class BoolMenuEntry : public StandardMenuEntry
{
  private:
    BoolSetting* const setting;
  
  public:
    BoolMenuEntry(BoolSetting *setting) : StandardMenuEntry(setting->getName()), setting(setting) { }

    virtual void action(Manager *manager, GCWKey key);
    virtual void render(Gfx* gfx, int x, int y);
};
  
class EnumMenuEntry : public StandardMenuEntry
{
  private:
    EnumSetting* const setting;
  
  public:
    EnumMenuEntry(EnumSetting* setting) : StandardMenuEntry(setting->getName()), setting(setting) { }
  
    virtual void action(Manager *manager, GCWKey key);
    virtual void render(Gfx* gfx, int x, int y);
};
  
class PathSettingMenuEntry : public StandardMenuEntry
{
  private:
    PathSetting* const setting;
    const std::string pathViewTitle;
  public:
    PathSettingMenuEntry(PathSetting *setting, const std::string& pathViewTitle) : StandardMenuEntry(setting->getName()), setting(setting), pathViewTitle(pathViewTitle) { }
  
    virtual void action(Manager *manager, GCWKey key);
    virtual void render(Gfx* gfx, int x, int y);
};
  
class PathMenuEntry : public MenuEntry
{
  private:
    Path path;
  public:
    PathMenuEntry(const Path& path) : path(path) { }
    
    virtual const std::string& name() { return path.value(); }
    virtual void action(Manager *manager, GCWKey key);
    virtual void render(Gfx* gfx, int x, int y);
};
  
  
class SystemMenuEntry : public SubMenuEntry
{
private:
  const System::Spec& system;
  SDL_Surface* const systemIcon;
  
public:
  SystemMenuEntry(const System::Spec& system, Menu *menu);
  SDL_Surface* icon() override { return systemIcon; }
};

class RomMenuEntry : public MenuEntry
{
  private:
    const RomEntry& rom;
  
  public:
    RomMenuEntry(const RomEntry& rom) : rom(rom) { }
    const std::string& name() override { return rom.name; }
    void render(Gfx* gfx, int x, int y) override;
    void action(Manager *manager, GCWKey key) override;

};

  
  
  
class Menu
{
  protected:
    std::string const caption;
    u16 spacing;
  
  public:
    Menu(const std::string& caption) : caption(caption), spacing(UI::MENU_DEFAULT_SPACING) { }
    void setSpacing(u16 spacing) { this->spacing = spacing; }
  
    virtual size_t count() const = 0;
    virtual MenuEntry* entryAt(u32 index) const = 0;
    virtual void render(Gfx* gfx, int offset, int size, int tx, int ty, int x, int y, int c = -1);

  
    const std::string& title() { return caption; }
};
  
class StandardMenu : public Menu
{
  protected:
    std::vector<std::unique_ptr<MenuEntry>> entries;
  
  public:
    StandardMenu(std::string caption) : Menu(caption) { }
  
    size_t count() const { return entries.size(); }
    void addEntry(MenuEntry *entry) { entries.push_back(std::unique_ptr<MenuEntry>(entry)); }
    MenuEntry* entryAt(u32 index) const { return entries[index].get(); }

};
  
class RomPathsMenu : public StandardMenu
{
  private:
    Persistence* const persistence;
  
  public:
    RomPathsMenu(std::string caption, Persistence *persistence) : StandardMenu(caption), persistence(persistence) { }
  
    void build();
  
};
  

class RomsMenu : public StandardMenu
{
public:
  RomsMenu(std::string caption, RomIteratorRange roms) : StandardMenu(caption)
  {
    for (RomIterator it = roms.first; it != roms.second; ++it)
    {
      const RomEntry& rom = it->second;
      entries.push_back(std::unique_ptr<MenuEntry>(new RomMenuEntry(rom)));
    }
  }
};
  
  
class SystemsMenu : public StandardMenu
{
  private:
  
  public:
    SystemsMenu(std::string caption, RomCollection *collection) : StandardMenu(caption)
    {
      const std::vector<System::Spec>& systems = System::getSystems();
      std::vector<System::Spec>::const_iterator it;
      
      for (it = systems.begin(); it != systems.end(); ++it)
      {
        const System::Spec& system = *it;
        RomIteratorRange roms = collection->getRomsForSystem(system);
        
        if (roms.first != roms.second)
          entries.push_back(std::unique_ptr<MenuEntry>(new SystemMenuEntry(system, new RomsMenu(system.name, roms))));
      }

    }
};
  
  class CoreMenuEntry : public StandardMenuEntry
  {
  private:
    const CoreHandle& core;
  public:
    CoreMenuEntry(const CoreHandle& core) : StandardMenuEntry(core.info.title()), core(core) { }
  };
  
  class CoresMenu : public StandardMenu
  {
  private:
    
  public:
    CoresMenu(Loader* loader) : StandardMenu("Cores")
    {
      auto& cores = loader->getCores();
      
      for (const CoreHandle& core : cores)
        entries.push_back(std::unique_ptr<MenuEntry>(new CoreMenuEntry(core)));
    }
  };
  
  
  class KeybindMenuEntry : public StandardMenuEntry
  {
  private:
    GCWKey key;
    
  public:
    KeybindMenuEntry(const std::string& name, GCWKey key) : StandardMenuEntry(name), key(key) { }
  };
}

#endif
