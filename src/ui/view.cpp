
#include "../core/manager.h"
#include "view.h"

using namespace gcw;




const ViewOffset View::TITLE_OFFSET = ViewOffset(10,10);
const ViewOffset View::MENU_OFFSET = ViewOffset(20,40);
const ViewOffset View::HELP_OFFSET = ViewOffset(10,230);


View::View(Manager *manager) : manager(manager), gfx(manager->getGfx()) { }

