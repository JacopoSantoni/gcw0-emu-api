#ifndef _ROM_LIST_H_
#define _ROM_LIST_H_

#include <vector>
#include <string>

#include "utils.h"
#include "defines.h"

namespace gcw {
  
  struct RomFile
  {
    std::string name;
    ConsoleType console;
  };

  class Manager;
  
  class RomCollection
  {
    private:
      Manager *manager;
      std::vector<std::string> paths;
    
    
    public:
      RomCollection(Manager *manager) : manager(manager) { }
      void addPath(std::string path) { paths.push_back(path); }
      void scan();
    
  };
  
}

#endif
