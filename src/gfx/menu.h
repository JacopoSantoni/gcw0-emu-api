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
    virtual bool isEnabled() const { return true; }
  
  protected:
    virtual void action(Manager *manager, GCWKey key) { }
  
  public:
    virtual void render(Gfx* gfx, int x, int y);
    void doAction(Manager* manager, GCWKey key) { if (isEnabled()) action(manager,key); }
};
  
class StandardMenuEntry : public MenuEntry
{
  protected:
    std::string caption;
    
  public:
    StandardMenuEntry() : caption() { }
    StandardMenuEntry(std::string caption) : caption(caption) { }
    template<typename S> void setCaption(S caption) { this->caption = std::forward<S>(caption); }
  
    virtual const std::string& name() { return caption; }
    virtual void action(Manager *manager, GCWKey key) { }
};
  
class LambdaMenuEntry : public StandardMenuEntry
{
  protected:
    std::function<void (Manager*)> lambda;
  
  public:
    LambdaMenuEntry(std::string caption, std::function<void (Manager*)> lambda) : StandardMenuEntry(caption), lambda(lambda) { }

    virtual void action(Manager *manager, GCWKey key) { if (key == MENU_ACTION_BUTTON) lambda(manager); }
};

class SubMenuEntry : public StandardMenuEntry
{
  protected:
    std::unique_ptr<Menu> menu;
 
  public:
    SubMenuEntry(std::string caption, Menu *menu) : StandardMenuEntry(caption+" >"), menu(menu) { }
  
    Menu *subMenu() { return menu.get(); }
  
    void action(Manager *manager, GCWKey key) override;
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
  
    const Path& getPath() { return path; }
    
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
    std::string caption;
    u16 spacing;
  
  public:
    Menu() : spacing(UI::MENU_DEFAULT_SPACING) { }
    Menu(const std::string& caption) : caption(caption), spacing(UI::MENU_DEFAULT_SPACING) { }
    void setSpacing(u16 spacing) { this->spacing = spacing; }
  
    virtual size_t count() const = 0;
    template<typename T> T* castedEntry(u32 index) const { return static_cast<T*>(entryAt(index)); }
    virtual MenuEntry* entryAt(u32 index) const = 0;
    virtual void render(Gfx* gfx, int offset, int size, int tx, int ty, int x, int y, int c = -1);

    void setTitle(const std::string& title) { this->caption = title; }
    const std::string& title() { return caption; }
};

class StandardMenu : public Menu
{
  protected:
    std::vector<std::unique_ptr<MenuEntry>> entries;
  
  public:
    StandardMenu() : Menu() { }
    StandardMenu(const std::string& caption) : Menu(caption) { }
  
    size_t count() const { return entries.size(); }
    void addEntry(MenuEntry *entry) { entries.push_back(std::unique_ptr<MenuEntry>(entry)); }
    void clear() { entries.clear(); }
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
  

  
  class CoreMenuEntry : public SubMenuEntry
  {
  private:
    CoreHandle& core;
  public:
    CoreMenuEntry(CoreHandle& core, Menu* menu) : SubMenuEntry(core.info.title(), menu), core(core) { }
    void action(Manager *manager, GCWKey key) override;
  };
  
  class CoresMenu : public StandardMenu
  {
  private:
    
  public:
    CoresMenu(Manager* manager);
  };
  
  
  class KeybindMenuEntry : public StandardMenuEntry
  {
  private:
    const ButtonSetting& setting;
    u16 spacing;
    std::optional<std::reference_wrapper<CoreHandle>> core;
    
  public:
    KeybindMenuEntry(const ButtonSetting& setting, u16 spacing = 50) : StandardMenuEntry(), setting(setting), spacing(spacing), core(std::nullopt) { }
    KeybindMenuEntry(const ButtonSetting& setting, CoreHandle& core, u16 spacing = 50) : StandardMenuEntry(), setting(setting), spacing(spacing), core(std::optional<std::reference_wrapper<CoreHandle>>(std::ref(core))) { }

    bool isEnabled() const override { return setting.isRebindable(); }
    
    void action(Manager *manager, GCWKey key) override;
    void render(Gfx* gfx, int x, int y) override;
  };
  
  
  
  
  class CoreMenu : public StandardMenu
  {
  private:
    
  public:
    CoreMenu() : StandardMenu() { }
    
    void build(CoreHandle& handle);
  };
}






#endif
