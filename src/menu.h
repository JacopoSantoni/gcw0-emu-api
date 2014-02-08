#ifndef _MENU_H_
#define _MENU_H_

#include "defines.h"

#include "gfx.h"
#include "settings.h"
#include "rom_collection.h"

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
    virtual const std::string& name() = 0;
  
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
    Menu* const menu;
 
  public:
    SubMenuEntry(std::string caption, Menu *menu) : StandardMenuEntry(caption+" >"), menu(menu) { }
  
    Menu *subMenu() { return menu; }
  
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
  
class ConsoleMenuEntry : public MenuEntry
{
private:
  ConsoleSpec* const console;
  SDL_Surface* const icon;
  
public:
  ConsoleMenuEntry(ConsoleSpec *console);
  virtual const std::string& name() { return console->ident; }
  virtual void render(Gfx* gfx, int x, int y);
};




class Menu
{
  protected:
    std::string const caption;
  
  public:
    Menu(std::string caption) : caption(caption) { }
  
    virtual size_t count() const = 0;
    virtual MenuEntry* entryAt(u32 index) = 0;
    virtual void render(Gfx* gfx, int tx, int ty, int x, int y);

  
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
    MenuEntry* entryAt(u32 index) { return entries[index].get(); }

};
  
class ConsoleMenu : public StandardMenu
{
  private:
  
  public:
    ConsoleMenu(std::string caption, std::vector<ConsoleSpec>* consoles) : StandardMenu(caption)
    {
      std::vector<ConsoleSpec>::iterator it;
      
      for (it = consoles->begin(); it != consoles->end(); ++it)
        entries.push_back(std::unique_ptr<MenuEntry>(new ConsoleMenuEntry(&(*it))));
    }
};
  
}

#endif
