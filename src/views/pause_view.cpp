#include "pause_view.h"

#include "../gfx/gfx.h"
#include "../gfx/ui.h"

using namespace std;
using namespace gcw;

class SlotSelectionMenuEntry : public MenuEntry
{
private:
  string caption;
  string fullCaption;
  u32& ref;
  
public:
  SlotSelectionMenuEntry(const string& caption, u32& ref) : MenuEntry(), caption(caption), ref(ref)
  {
    updateCaption();
  }
  
  const std::string& name() override { return fullCaption; }
  
  void action(Manager *manager, GCWKey key) override
  {
    if (key == GCW_KEY_LEFT)
    {
      if (ref > 0)
        --ref;
      else
        ref = MAX_SAVE_SLOTS - 1;

    }
    else if (key == GCW_KEY_RIGHT || key == MENU_ACTION_BUTTON)
    {
      if (ref < MAX_SAVE_SLOTS - 1)
        ++ref;
      else
        ref = 0;
    }
    
    updateCaption();
  }
  
  void updateCaption()
  {
    fullCaption.assign(caption + to_string(ref));
  }
  
};


u32 PauseView::PauseEntryList::current() const { return currentIndex; }
u32 PauseView::PauseEntryList::count() const { return view->menu->count(); }
void PauseView::PauseEntryList::set(u32 i) { currentIndex = i; }
MenuEntry* PauseView::PauseEntryList::get(u32 i) { return view->menu->entryAt(offset+i); }

PauseView::PauseView(Manager* manager) : View(manager), currentSaveSlot(0), currentLoadSlot(0), list(this)
{
  StandardMenu* menu = new StandardMenu("Pause Menu");
  
  this->menu = unique_ptr<Menu>(menu);
  
  menu->addEntry(new StandardMenuEntry("Back to game"));
  menu->addEntry(new SlotSelectionMenuEntry("Save to slot ", currentSaveSlot));
  menu->addEntry(new SlotSelectionMenuEntry("Load from slot ", currentLoadSlot));
  menu->addEntry(new StandardMenuEntry("Settings"));
  menu->addEntry(new StandardMenuEntry("Soft Reset"));
  menu->addEntry(new StandardMenuEntry("Stop"));
  menu->setSpacing(10);
}

void PauseView::handleEvents()
{
  SDL_Event event;
  
  while (SDL_PollEvent(&event))
  {
    switch(event.type)
    {
      case SDL_KEYDOWN:
      {
        GCWKey key = static_cast<GCWKey>(event.key.keysym.sym);
        switch (key)
        {
          case GCW_KEY_DOWN: list.down(); break;
          case GCW_KEY_UP: list.up(); break;
            
          case GCW_KEY_L: list.prevPage(); break;
          case GCW_KEY_R: list.nextPage(); break;
          
          
          case GCW_KEY_RIGHT:
          case GCW_KEY_LEFT:
          case MENU_ACTION_BUTTON:
            list.selected()->action(manager, key); break;
            
          default: break;
        }
      }
    }
  }

}

void PauseView::render()
{
  menu->render(gfx, list.minOffset(), list.getDisplayedAmount(), UI::TITLE_OFFSET.x, UI::TITLE_OFFSET.y, UI::MENU_OFFSET.x, UI::MENU_OFFSET.y, list.current());
}