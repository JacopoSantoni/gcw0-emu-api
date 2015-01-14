#ifndef __PAUSE_VIEW_H__
#define __PAUSE_VIEW_H__

#include "menu_view.h"
#include "../gfx/ui.h"
#include "../gfx/menu.h"

#include <memory>

namespace gcw
{
  class Manager;
  class Menu;
  class MenuEntry;
  struct RomEntry;
  class CoreInterface;
  
  class PauseView : public MenuView
  {
  private:
    SaveSlot currentSaveSlot;
    SaveSlot currentLoadSlot;
    
    const RomEntry* rom;
    const CoreInterface* core;
    
  public:
    PauseView(Manager* manager, std::unique_ptr<Menu>& root);
    
    void reset()
    {
      //menu.reset(nullptr);
      rom = nullptr;
      core = nullptr;
      currentSaveSlot = 0;
      currentLoadSlot = 0;
    }
    
    void initialize(const CoreInterface* core, const RomEntry* rom);
    
    //void activated() override;
    void backActionOnRoot() override;
  };
  
}


#endif