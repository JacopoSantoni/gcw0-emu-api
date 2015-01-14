#include "dialog_view.h"

#include "../gfx/ui.h"
#include "../gfx/gfx.h"
#include "../common/keys.h"

using namespace std;
using namespace gcw;

DialogView::DialogView(Manager* manager) : View(manager)
{
  
}

void DialogView::handleEvents()
{
  SDL_Event event;
  
  while (SDL_PollEvent(&event))
  {
    switch(event.type)
    {
      case SDL_KEYDOWN:
      {
        GCWKey key = static_cast<GCWKey>(event.key.keysym.sym);

        if (type == MESSAGE)
        {
          yesLambda();
          break;
        }
        else if (type == CONFIRM)
        {
          if (key == MENU_ACTION_BUTTON)
            yesLambda();
          else
            noLambda();
        }
        
        break;
      }
    }
  }
}

void DialogView::render()
{
  gfx->print(UI::DIALOG_MESSAGE.x, UI::DIALOG_MESSAGE.y, true, Font::bigFont, message);
  
  string help;
  
  if (type == MESSAGE)
    help = "Press any key to close this message.";
  else if (type == CONFIRM)
    help = string("Press ") + Keys::nameForKey(MENU_ACTION_BUTTON) + " to confirm,\nany other key to cancel.";
    
  gfx->print(UI::DIALOG_HELP.x, UI::DIALOG_HELP.y, true, Font::bigFont, help);
}