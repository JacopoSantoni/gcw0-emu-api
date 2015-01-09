#include "loading_view.h"

#include "../gfx/gfx.h"

using namespace gcw;

LoadingView::LoadingView(Manager* manager) : View(manager), percent(0.0f) { }


void LoadingView::handleEvents()
{
  SDL_Event event;
  while (SDL_PollEvent(&event));
}

void LoadingView::render()
{
  //percent = std::min(1.0f, percent + 0.005f);
  
  static const Size barSize = Size(WIDTH*0.80f,30);
  static const Offset barOffset = Offset((WIDTH - barSize.x)/2, (HEIGHT - barSize.y)/2);
  
  static const u16 barEmptyFill = Gfx::ccc<u16>(50, 0, 0);
  static const u16 barFullFill = Gfx::ccc<u16>(180, 0, 0);
  static const u16 barStroke = Gfx::ccc<u16>(220, 0, 0);
  
  Size barFullSize = Size(barSize.x * percent, barSize.y);

  
  gfx->rectFill(barOffset.x, barOffset.y, barSize.x, barSize.y, barEmptyFill);
  gfx->rect(barOffset.x, barOffset.y, barSize.x, barSize.y, barStroke);
  gfx->rectFill(barOffset.x, barOffset.y, barFullSize.x, barFullSize.y, barStroke);
  gfx->rect(barOffset.x, barOffset.y, barFullSize.x, barFullSize.y, barFullFill);
  
  gfx->printf(barOffset.x+barSize.x/2, barOffset.y+barSize.y/2, true, Font::bigFont, "%.0f%%", percent*100);
  
}

void LoadingView::resetForFile(const std::string &name)
{
  percent = 0.0f;
}