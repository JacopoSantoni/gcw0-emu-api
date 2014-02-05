#include "menu.h"

#include "menu_view.h"

using namespace gcw;

void SubMenuEntry::action(MenuView *view, GCWKey key)
{
  if (key == GCW_KEY_RIGHT)
    view->enterSubmenu(this);
}