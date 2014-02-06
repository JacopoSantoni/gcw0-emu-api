#include "manager.h"

/*#ifdef MAKE
int SDL_main(int argc, char **argv)
#else
int run(int argc, char **argv)
#endif*/
int run(int argc, char **argv)
{
  gcw::Manager manager;
  
  manager.init();
  manager.run();

  /*if (argc > 1)
  {
    gcw::CoreHandle *core = loader.coreForFile(argv[1]);
    if (core)
    {
      loader.loadCore(core->info.ident);
      loader.getCore()->run(argc, argv);
    }
  }*/
  
  return 0;
}

#ifdef __linux__
int main(int argc, char **argv)
{
  SDL_main(argc,argv);
}
#endif
