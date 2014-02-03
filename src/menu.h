#ifndef _MENU_H_
#define _MENU_H_

#include "defines.h"

#include <string>
#include <vector>
#include <memory>

class MenuEntry
{
  private:
  
  public:
    virtual std::string name();
};

class Menu
{
  private:
    std::vector<std::unique_ptr<MenuEntry>> entries;
  
  public:
    Menu() { }
  
    size_t count() { return entries.size(); }
    void addEntry(MenuEntry *entry) { entries.push_back(std::unique_ptr<MenuEntry>(entry)); }
    MenuEntry* entryAt(u32 index) { return entries[index].get(); }
  
};

#endif
