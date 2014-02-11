#ifndef _ROM_LIST_H_
#define _ROM_LIST_H_

#include <vector>
#include <string>
#include <unordered_set>
#include <unordered_map>

#include "utils.h"
#include "defines.h"

namespace gcw {
  
  
  struct SystemSpec
  {
    std::string ident;
    std::string name;
    std::vector<std::string> extensions;
    
    SystemSpec(std::string ident, std::string &&name, std::vector<std::string> &&extensions) : ident(ident), name(name), extensions(extensions) { }
  };
  
  struct RomEntry
  {
    const std::string name;
    const std::string ext;
    SystemSpec *system;
    const Path *path;
    
    RomEntry(const std::string name, const std::string ext, SystemSpec *system, const Path* path) : name(name), ext(ext), system(system), path(path) { }
  };
  
  typedef std::unordered_multimap<SystemSpec*, RomEntry> RomMap;
  typedef RomMap::iterator RomIterator;
  typedef std::pair<RomIterator, RomIterator> RomIteratorRange;

  class Manager;
  
  class RomCollection
  {
    private:
      std::vector<SystemSpec> specs;
    
      Manager *manager;
    
      RomMap roms;
    
    
    public:
      RomCollection(Manager *manager) : manager(manager) { }
      void scan();
    
      std::vector<SystemSpec>* getSystems() { return &specs; }
      RomIteratorRange getRomsForSystem(SystemSpec* spec) { return roms.equal_range(spec); }    
  };
  
}

#endif
