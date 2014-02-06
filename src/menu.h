#ifndef _MENU_H_
#define _MENU_H_

#include "defines.h"

#include "settings.h"

#include <string>
#include <vector>
#include <memory>

namespace gcw {
  
class Menu;
class MenuView;

class MenuEntry
{
  protected:
    std::string caption;
  
  public:
    MenuEntry() { }
    MenuEntry(std::string caption) : caption(caption) { }
    virtual const std::string& name() { return caption; }
  
    virtual void action(MenuView *view, GCWKey key) { }
};

class SubMenuEntry : public MenuEntry
{
  protected:
    Menu *menu;
 
  public:
    SubMenuEntry(std::string caption, Menu *menu) : MenuEntry(caption), menu(menu) { }
  
    Menu *subMenu() { return menu; }
  
    virtual void action(MenuView *view, GCWKey key);
};
  
  class BoolMenuEntry : public MenuEntry
{
  private:
    BoolSetting* setting;
  
  public:
    BoolMenuEntry(BoolSetting *setting) : MenuEntry(setting->getName()+" "+(setting->getValue()?"true":"false")), setting(setting) { }

    virtual void action(MenuView *view, GCWKey key);
};




class Menu
{
  private:
    std::string caption;
    std::vector<std::unique_ptr<MenuEntry>> entries;
  
  public:
    Menu(std::string caption) : caption(caption) { }
  
    size_t count() { return entries.size(); }
    void addEntry(MenuEntry *entry) { entries.push_back(std::unique_ptr<MenuEntry>(entry)); }
    MenuEntry* entryAt(u32 index) { return entries[index].get(); }
  
    const std::string& title() { return caption; }
};
  
}

#endif
