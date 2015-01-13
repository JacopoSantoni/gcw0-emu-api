#include "dialog_view.h"

using namespace gcw;

DialogView::DialogView(Manager* manager) : View(manager)
{
  
}

void DialogView::handleEvents()
{
  SDL_Event event;
  
  while (SDL_PollEvent(&event))
  {
    
  }
}

void DialogView::render()
{
  
}