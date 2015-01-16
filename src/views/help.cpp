#include "help.h"

#include "../core/manager.h"
#include "../gfx/gfx.h"
#include "../gfx/ui.h"
#include "../common/keys.h"

#include <sstream>

using namespace gcw;
using namespace std;

Help::Help(Manager* manager) : manager(manager) { }


void Help::render()
{
  Gfx* gfx = manager->getGfx();
  
  gfx->print(WIDTH/2, UI::HELP_OFFSET.y, true, Font::bigFont, message);
  
}

void Help::clear()
{
  message.clear();
}

void Help::buildMessage(const std::vector<Entry>& entries)
{
  stringstream ss;
  
  for (const auto& e : entries)
  {
    for (const auto& k : e.key)
      ss << Keys::nameForKey(k);
    
    ss << ": " << e.text << "    ";
  }
  
  message = ss.str();
}


void Help::set(HelpType type)
{
  switch (type) {
    case HelpType::SUBMENU:
    {
      buildMessage({{{MENU_ACTION_BUTTON}, "enter menu"}});
      break;
    }
      
    case HelpType::BOOL_SETTING:
    {
      buildMessage({{{MENU_ACTION_BUTTON,GCW_KEY_LEFT,GCW_KEY_RIGHT}, "toggle setting"}});
      break;
    }
      
    case HelpType::ENUM_SETTING:
    {
      buildMessage({{{MENU_ACTION_BUTTON,GCW_KEY_LEFT,GCW_KEY_RIGHT}, "change setting"}});
      break;
    }
      
    case HelpType::PATH_SETTING:
    {
      buildMessage({{{MENU_ACTION_BUTTON}, "edit path"}});
      break;
    }
      
    case HelpType::SETTING_CANT_BE_CHANGED: message = "Setting can't be changed now"; break;
    case HelpType::LAUNCH_ROM:       buildMessage({{{MENU_ACTION_BUTTON}, "launch rom"}}); break;
      
      
    case HelpType::NONE:
    {
      clear();
      break;
    }
      
  }
}