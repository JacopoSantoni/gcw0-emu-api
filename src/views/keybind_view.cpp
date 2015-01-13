#include "keybind_view.h"

#include "../gfx/gfx.h"
#include "../gfx/ui.h"

#include <sstream>

using namespace std;
using namespace chrono;
using namespace gcw;

void KeybindView::init(KeybindView::rebind_lambda lambda, KeybindView::cancel_lambda clambda, const std::string& caption)
{
  this->canBeMultiKey = false;
  this->lambda = lambda;
  this->clambda = clambda;
  this->caption = caption;
}

void KeybindView::initm(KeybindView::mrebind_lambda mlambda, KeybindView::cancel_lambda clambda, const std::string& caption)
{
  this->canBeMultiKey = true;
  this->mlambda = mlambda;
  this->clambda = clambda;
  this->caption = caption;
  mask = 0;
}

void KeybindView::handleEvents()
{
  SDL_Event event;
  
  while (SDL_PollEvent(&event))
  {
    // TODO: analog is not managed for now here
    
    switch(event.type)
    {
      case SDL_KEYDOWN:
      {
        GCWKey key = static_cast<GCWKey>(event.key.keysym.sym);
        
        if (!canBeMultiKey)
          lambda(key);
        else
          mask |= 1 << Keys::indexForKey(key);
        
        start = steady_clock::now();
        break;
      }
        
      case SDL_KEYUP:
      {
        GCWKey key = static_cast<GCWKey>(event.key.keysym.sym);

        mask &= ~ (1 << Keys::indexForKey(key));
        
        start = steady_clock::now();
        break;
      }
    }
  }
}

void KeybindView::render()
{
  if (!canBeMultiKey)
    gfx->printf(UI::REBIND_KEY_INFO.x, UI::REBIND_KEY_INFO.y, true, Font::bigFont, "Please press a key or wait %d seconds to cancel", UI::REBIND_TIMEOUT_MSECS/1000);
  else
  {
    gfx->printf(UI::REBIND_KEY_INFO.x, UI::REBIND_KEY_INFO.y, true, Font::bigFont, "Please press a key (or combination)\nand wait %d seconds to apply.\nWait without pressing anything to cancel.", UI::REBIND_TIMEOUT_MSECS/1000);
   
    if (mask)
    {
      stringstream ss;
      bool first = true;
      
      for (int i = 0; i < GCW_KEY_COUNT; ++i)
      {
        if (mask & (1 << i))
        {
          if (!first) ss << " ";
          
          ss << Keys::nameForKey(Keys::keyForIndex(i));
          first = false;
        }
      }
      
      gfx->print(UI::REBIND_KEY_STATUS.x, UI::REBIND_KEY_STATUS.y, true, Font::bigFont, ss.str());
    }
    
  }
  
  
  gfx->print(UI::REBIND_KEY_CAPTION.x, UI::REBIND_KEY_CAPTION.y, true, Font::bigFont, caption);

  
  if (duration_cast<milliseconds>(steady_clock::now() - start).count() > UI::REBIND_TIMEOUT_MSECS)
  {
    if (!canBeMultiKey)
      clambda();
    else
    {
      if (!mask)
        clambda();
      else
        mlambda(mask);
    }
  }
}

void KeybindView::activated()
{
  UI::disableKeyRepeat();
  start = steady_clock::now();
}

void KeybindView::deactivated()
{
  UI::enableKeyRepeat();
}