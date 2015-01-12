#ifndef __PAUSE_VIEW_H__
#define __PAUSE_VIEW_H__

#include "../gfx/view.h"
#include "../gfx/menu.h"
#include "../gfx/ui.h"

#include <memory>

namespace gcw
{
  class Manager;
  class Menu;
  
  class PauseView : public View
  {
  private:
    std::unique_ptr<Menu> menu;
    u32 currentSaveSlot;
    u32 currentLoadSlot;
    
    
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
    
    //void activated() override;
    
    void backToGame();
    void resetGame();
    void backToManager();
    
    friend class PauseEntryList;
  };
  
}


#endif