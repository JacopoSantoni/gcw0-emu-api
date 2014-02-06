#ifndef _ROM_LIST_H_
#define _ROM_LIST_H_

#include <vector>
#include <string>
#include <unordered_set>
#include <unordered_map>

#include "utils.h"
#include "defines.h"

namespace gcw {
  
  
  struct ConsoleSpec
  {
    std::string ident;
    std::string name;
    std::vector<std::string> extensions;
    
    ConsoleSpec(std::string ident, std::string &&name, std::vector<std::string> &&extensions) : ident(ident), name(name), extensions(extensions) { }
  };
  
  struct RomEntry
  {
    std::string name;
    std::string ext;
    ConsoleSpec *console;
    std::string *path;
  };
  
  typedef std::unordered_multimap<ConsoleSpec*, RomEntry> RomMap;

  class Manager;
  
  class RomCollection
  {
    private:
      std::vector<ConsoleSpec> specs;
    
      Manager *manager;
      std::vector<std::string> paths;
    
      RomMap roms;
    
    
    public:
      RomCollection(Manager *manager) : manager(manager) { }
      void addPath(std::string path) { paths.push_back(path); }
      void scan();
    
  };
  
}

#endif
