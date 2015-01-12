#include "pause_view.h"

#include "../core/manager.h"

#include "../gfx/gfx.h"
#include "../gfx/ui.h"

using namespace std;
using namespace gcw;

class SlotSelectionMenuEntry : public StandardMenuEntry
{
private:
  Manager* manager;
  string caption;
  SaveSlot& ref;
  bool enabled;
  bool isSave;
  
public:
  SlotSelectionMenuEntry(Manager* manager, const string& caption, SaveSlot& ref, bool isSave) : StandardMenuEntry(), manager(manager), caption(caption), ref(ref), enabled(true), isSave(isSave)
  {
    updateCaption();
  }
    
  void action(Manager *manager, GCWKey key) override
  {
    if (key == GCW_KEY_LEFT)
    {
      if (ref > 0)
        --ref;
      else
        ref = MAX_SAVE_SLOTS - 1;

    }
    else if (key == GCW_KEY_RIGHT)
    {
      if (ref < MAX_SAVE_SLOTS - 1)
        ++ref;
      else
        ref = 0;
    }
    else if (key == MENU_ACTION_BUTTON)
    {
      if (isSave)
      {
        manager->stateSave(ref);
        manager->resumeEmulation();
        //updateCaption();
      }
      else
      {
        manager->stateLoad(ref);
        manager->resumeEmulation();
      }
    }
    
    updateCaption();
  }
  
  void setEnabled(bool enabled) { this->enabled = enabled; }
  bool isEnabled() { return enabled; }
  
  void updateCaption() {
    
    if (manager->getCurrentCore() && manager->getCurrentRom())
    {
      Path path = manager->getPersistence()->savePath(manager->getCurrentCore()->info(), manager->getCurrentRom(), ref);
      if (path.exists())
      {
        setCaption(caption + to_string(ref) + " (" + Text::dateToString(path.timeModified())+")");
        return;
      }
    }

    setCaption(caption + to_string(ref));
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
  
  menu->addEntry(new LambdaMenuEntry("Back to game", [](Manager* manager){ manager->resumeEmulation(); }));
  menu->addEntry(new SlotSelectionMenuEntry(manager, "Save to slot ", currentSaveSlot, true));
  menu->addEntry(new SlotSelectionMenuEntry(manager, "Load from slot ", currentLoadSlot, false));
  menu->addEntry(new StandardMenuEntry("Settings"));
  menu->addEntry(new StandardMenuEntry("Soft Reset"));
  menu->addEntry(new LambdaMenuEntry("Back to roms", [](Manager* manager) { manager->switchView(View::Type::MENU); }));
  menu->setSpacing(10);
}

void PauseView::initialize(const CoreInterface* core, const RomEntry* rom)
{
  list.set(0);
  
  menu->setTitle(rom->name + " on " + core->info().details.name);
  
  SlotSelectionMenuEntry* save = menu->castedEntry<SlotSelectionMenuEntry>(1);
  SlotSelectionMenuEntry* load = menu->castedEntry<SlotSelectionMenuEntry>(2);
  
  save->setEnabled(core->hasFeature(CoreFeature::CAN_SAVE_STATE));
  load->setEnabled(core->hasFeature(CoreFeature::CAN_SAVE_STATE));
  save->updateCaption();
  load->updateCaption();
  
  this->core = core;
  this->rom = rom;
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
            list.selected()->doAction(manager, key); break;
            
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