#include "emu_interface.h"

#include <SDL.h>

#include "loader.h"
#include "emu_interface.h"

#include "dlfcn.h"


int main(int argc, char **argv)
{
  gcw::Loader loader;
  
  loader.scan();
  
  if (argc > 1)
  {
    gcw::CoreHandle *core = loader.coreForFile(argv[1]);
    if (core)
    {
      loader.loadCore(core->info.ident);
      loader.getCore()->run(argc, argv);
    }
  }
  
  return 0;
}
