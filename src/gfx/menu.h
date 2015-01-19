#ifndef _MENU_H_
#define _MENU_H_

#include "../common/defines.h"

#include "gfx.h"
#include "ui.h"
#include "../views/help.h"
#include "../data/settings.h"
#include "../data/rom_collection.h"
#include "../data/persistence.h"
#include "../core/loader.h"


#include <string>
#include <vector>
#include <memory>

namespace gcw {

class BlitterFactory;
  
class Menu;
class MenuView;
  
enum class HelpType : u16;

class MenuEntry
{
public:
  MenuEntry() { }
  virtual ~MenuEntry() { }
  virtual const std::string& name() const = 0;
  virtual SDL_Surface* icon() { return nullptr; }
  virtual bool isEnabled() const { return true; }
  virtual HelpType help() const = 0;

protected:
  virtual void action(Manager *manager, GCWKey key) { }

public:
  virtual void render(Gfx* gfx, int x, int y, bool isSelected = false);
  void doAction(Manager* manager, GCWKey key) { if (isEnabled()) action(manager,key); }
};
  
class StandardMenuEntry : public MenuEntry
{
protected:
  HelpType hhelp;
  std::string caption;
  
public:
  StandardMenuEntry() : hhelp(HelpType::NONE), caption() { }
  StandardMenuEntry(std::string caption) : hhelp(HelpType::NONE), caption(caption) { }
  HelpType help() const override { return hhelp; };
  
  template<typename S> void setCaption(S caption) { this->caption = std::forward<S>(caption); }


  const std::string& name() const override { return caption; }
  void action(Manager *manager, GCWKey key) override { }
};
  
class LambdaMenuEntry : public StandardMenuEntry
{
protected:
  std::function<void (Manager*)> lambda;

public:
  LambdaMenuEntry(std::string caption, std::function<void (Manager*)> lambda) : StandardMenuEntry(caption), lambda(lambda) { }

  void action(Manager *manager, GCWKey key) override { if (key == MENU_ACTION_BUTTON) lambda(manager); }
};

class SubMenuEntry : public StandardMenuEntry
{
protected:
  std::unique_ptr<Menu> menu;

public:
  SubMenuEntry(std::string caption, Menu *menu) : StandardMenuEntry(caption+" \x15"), menu(menu) { }
  HelpType help() const override { return HelpType::SUBMENU; }

  Menu *subMenu() { return menu.get(); }
  void action(Manager *manager, GCWKey key) override;
};
  
class SettingMenuEntry : public StandardMenuEntry
{
private:
  const u16 totalSpacing = 10;
  u16 valueTextWidth;
  
protected:
  const CoreHandle& handle;
  
public:
  SettingMenuEntry(const std::string& name, const CoreHandle& handle) : StandardMenuEntry(name), handle(handle) { }
  virtual const std::string getValueName() const = 0;
  virtual bool canBeModifiedAtRuntime() const = 0;
  bool canBeModified() const;
  void setValueTextWidth(u16 valueTextWidth) { this->valueTextWidth = valueTextWidth + 10; }
  
  void action(Manager *manager, GCWKey key) override;
  void render(Gfx* gfx, int x, int y, bool isSelected) override;
};
  
class RealSettingMenuEntry : public SettingMenuEntry
{
protected:
  const Setting& setting;
  void setValue(Manager* manager, const std::string& value);
  const std::string& getValue() const;
  
public:
  RealSettingMenuEntry(const CoreHandle& handle, const Setting& setting) : SettingMenuEntry(setting.name, handle), setting(setting) { }

  bool canBeModifiedAtRuntime() const override { return setting.canBeModifiedAtRuntime; }
  const std::string getValueName() const override { return getValue(); }
};
  
class BoolMenuEntry : public RealSettingMenuEntry
{
public:
  BoolMenuEntry(const CoreHandle& handle, const Setting& setting) : RealSettingMenuEntry(handle, setting) {
    setValueTextWidth(std::max(Font::bigFont.stringWidth("Yes"), Font::bigFont.stringWidth("No")));
  }
  HelpType help() const override { return canBeModified() ? HelpType::BOOL_SETTING : HelpType::SETTING_CANT_BE_CHANGED; }
  const std::string getValueName() const override { return getValue() == "true" ? "Yes" : "No"; }

  
  void action(Manager *manager, GCWKey key) override;
};
  
class EnumMenuEntry : public RealSettingMenuEntry
{
public:
  EnumMenuEntry(const CoreHandle& handle, const Setting& setting) : RealSettingMenuEntry(handle, setting)
  {
    u16 maxWidth = 0;
    for (const auto& name : setting.values)
    {
      u16 length = Font::bigFont.stringWidth(name.c_str());
      maxWidth = std::max(maxWidth, length);
    }

    setValueTextWidth(maxWidth);
  }
  HelpType help() const override { return canBeModified() ? HelpType::ENUM_SETTING : HelpType::SETTING_CANT_BE_CHANGED; }

  void action(Manager *manager, GCWKey key) override;
};
  
class PathSettingMenuEntry : public RealSettingMenuEntry
{
public:
  PathSettingMenuEntry(const CoreHandle& handle, const Setting& setting) : RealSettingMenuEntry(handle, setting) { }
  HelpType help() const override { return canBeModified() ? HelpType::PATH_SETTING : HelpType::SETTING_CANT_BE_CHANGED; }
  
  virtual void action(Manager *manager, GCWKey key);
  void render(Gfx* gfx, int x, int y, bool isSelected) override;
};
  
class BlitterMenuEntry : public SettingMenuEntry
{
private:
  std::vector<const std::string> blitters;
  decltype(blitters)::iterator current;
  
public:
  BlitterMenuEntry(const CoreHandle& handle, std::vector<const std::string>& blitters);
  
  const std::string getValueName() const override;
  bool canBeModifiedAtRuntime() const override { return blitters.size() > 1; }
  
  void action(Manager *manager, GCWKey key) override;
};
  
class PathMenuEntry : public MenuEntry
{
private:
  const Path& path;
public:
  PathMenuEntry(const Path& path) : path(path) { }
  virtual HelpType help() const { return HelpType::NONE; }

  const Path& getPath() { return path; }
  
  virtual const std::string& name() const { return path.value(); }
  virtual void action(Manager *manager, GCWKey key);
  void render(Gfx* gfx, int x, int y, bool isSelected) override;
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
  const std::string& name() const override { return rom.name; }
  void render(Gfx* gfx, int x, int y, bool isSelected) override;
  void action(Manager *manager, GCWKey key) override;
  HelpType help() const override { return HelpType::LAUNCH_ROM; }
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
  
    virtual void addEntry(MenuEntry *entry) = 0;
    virtual size_t count() const = 0;
    template<typename T> T* castedEntry(u32 index) const { return static_cast<T*>(entryAt(index)); }
    virtual MenuEntry* entryAt(u32 index) const = 0;
    virtual void clear() = 0;
  
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
    void addEntry(MenuEntry *entry) override { entries.push_back(std::unique_ptr<MenuEntry>(entry)); }
    void clear() override { entries.clear(); }
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
  void render(Gfx* gfx, int x, int y, bool isSelected) override;
};

class CoreMenu : public StandardMenu
{
private:
  
public:
  CoreMenu() : StandardMenu() { }
  
  void build(CoreHandle& handle);
};
  
  
class Menus
{
private:
  std::unique_ptr<Menu> mainMenu;
  std::unique_ptr<CoreMenu> coreMenu;
  std::unique_ptr<Menu> coresMenu;
  std::unique_ptr<Menu> pauseMenu;
  std::unique_ptr<Menu> systemsMenu;
  
public:
  Menus() :
  coreMenu(new CoreMenu()),
  coresMenu(new StandardMenu("Cores")),
  pauseMenu(new StandardMenu("Pause Menu")),
  systemsMenu(new StandardMenu("Browse by System"))
  
  { }
  
  /* TODO: maybe unnecessary in final version? */
  void setMainMenu(Menu* menu) { mainMenu.reset(menu); }
  
  CoreMenu* getCoreMenu() { return coreMenu.get(); }
  Menu* getCoresMenu() { return coresMenu.get(); }
  Menu* getMainMenu() { return mainMenu.get(); }
  Menu* getPauseMenu() { return pauseMenu.get(); }
  Menu* getSystemsMenu() { return systemsMenu.get(); }
  
  void buildCoresMenu(Manager* manager);
  void buildSystemsMenu(const RomCollection* collection);
  
  friend class Manager;
};
  
  
}






#endif
