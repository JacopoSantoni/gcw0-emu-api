#ifndef _ROM_LIST_H_
#define _ROM_LIST_H_

#include <vector>
#include <string>
#include <unordered_set>
#include <unordered_map>
#include <functional>
#include <algorithm>

#include "../common/utils.h"
#include "../common/defines.h"

namespace gcw {
  
  

  
  struct RomEntry
  {
    const std::string name;
    const Path path;
    const System::Spec& system;
    
    RomEntry(const std::string& name, const System::Spec& system, const Path& path) : name(name), path(path), system(system) { }
    
    std::string extension() const { return path.extension(); }
    std::string folder() const { return path.folder().value(); }
    std::string fileName() const { return path.plainName(); }
  };
  
  typedef std::unordered_multimap<std::reference_wrapper<const System::Spec>, RomEntry> RomMap;
  typedef RomMap::const_iterator RomIterator;
  typedef std::pair<RomIterator, RomIterator> RomIteratorRange;
  


  class Manager;
  
  class RomCollection
  {
    private:
      Manager *manager;
    
      RomMap roms;
    
    
    public:
      RomCollection(Manager *manager) : manager(manager) { }
      void scan();
    
      RomIteratorRange getRomsForSystem(const System::Spec& spec) const { return roms.equal_range(std::reference_wrapper<const System::Spec>(spec)); }
  };
  
}

#endif
