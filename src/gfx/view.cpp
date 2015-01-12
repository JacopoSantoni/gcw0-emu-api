
#include "../core/manager.h"
#include "view.h"

using namespace gcw;

View::View(Manager *manager) : manager(manager), gfx(manager->getGfx()) { }

