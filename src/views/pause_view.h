#ifndef __PAUSE_VIEW_H__
#define __PAUSE_VIEW_H__

#include "../gfx/view.h"
#include "../gfx/ui.h"

#include <memory>

namespace gcw
{
  class Manager;
  class Menu;
  class MenuEntry;
  struct RomEntry;
  class CoreInterface;
  
  class PauseView : public View
  {
  private:
    std::unique_ptr<Menu> menu;
    SaveSlot currentSaveSlot;
    SaveSlot currentLoadSlot;
    
    const RomEntry* rom;
    const CoreInterface* core;
    
    
    class PauseEntryList : public OffsettableList<MenuEntry*>
    {
    private:
      PauseView* view;
      u32 currentIndex;
      
    public:
      PauseEntryList(PauseView* view) : OffsettableList(UI::LIST_SIZE), view(view), currentIndex(0) { }
      
      u32 current() const;
      u32 count() const;
      void set(u32 i);
      MenuEntry* get(u32 i);
      
      MenuEntry* selected() { return get(currentIndex); }
    } list;
    
  public:
    PauseView(Manager* manager);
    
    virtual void handleEvents();
    virtual void render();
    
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
    
    void backToGame();
    void resetGame();
    void backToManager();
    
    friend class PauseEntryList;
  };
  
}


#endif