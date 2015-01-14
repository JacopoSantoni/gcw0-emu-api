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
  bool isEnabled() const override { return enabled; }
  
  void updateCaption() {
    
    if (manager->getCurrentCore() && manager->getCurrentRom())
    {
      Path path = manager->getPersistence()->savePath(manager->getCurrentCoreInfo(), manager->getCurrentRom(), ref);
      if (path.exists())
      {
        setCaption(caption + Text::to_string(ref) + " (" + Text::dateToString(path.timeModified())+")");
        return;
      }
    }

    setCaption(caption + Text::to_string(ref));
  }
};

PauseView::PauseView(Manager* manager, std::unique_ptr<Menu>& root) : MenuView(manager, root), currentSaveSlot(0), currentLoadSlot(0)
{
  Menu* menu = manager->getMenus()->getPauseMenu();
  
  menu->addEntry(new LambdaMenuEntry("Back to game", [](Manager* manager){ manager->resumeEmulation(); }));
  menu->addEntry(new SlotSelectionMenuEntry(manager, "Save to slot ", currentSaveSlot, true));
  menu->addEntry(new SlotSelectionMenuEntry(manager, "Load from slot ", currentLoadSlot, false));
  menu->addEntry(new SubMenuEntry("Core Settings", manager->getMenus()->getCoreMenu()));
  menu->addEntry(new LambdaMenuEntry("Soft Reset", [](Manager* manager) {
    if (manager->getCurrentCore()->hasFeature(CoreFeature::CAN_SOFT_RESET))
    {
      manager->softReset();
      manager->resumeEmulation();
    }
  }));
  menu->addEntry(new LambdaMenuEntry("Back to roms", [](Manager* manager) { manager->switchView(View::Type::MENU); }));
  menu->setSpacing(10);
}

void PauseView::initialize(const CoreInterface* core, const RomEntry* rom)
{
  resetPosition();
  
  root->setTitle(rom->name + " on " + core->info().details.name);
  
  SlotSelectionMenuEntry* save = root->castedEntry<SlotSelectionMenuEntry>(1);
  SlotSelectionMenuEntry* load = root->castedEntry<SlotSelectionMenuEntry>(2);
  
  save->setEnabled(core->hasFeature(CoreFeature::CAN_SAVE_STATE));
  load->setEnabled(core->hasFeature(CoreFeature::CAN_SAVE_STATE));
  save->updateCaption();
  load->updateCaption();
  
  this->core = core;
  this->rom = rom;
  
  resetToRoot();
}

void PauseView::backActionOnRoot()
{
  manager->resumeEmulation();
}
