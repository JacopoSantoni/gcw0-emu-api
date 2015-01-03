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
    const std::string ext;
    const System::Spec& system;
    const Path path;
    
    RomEntry(const std::string& name, const std::string& ext, const System::Spec& system, const Path path) : name(name), ext(ext), system(system), path(path) { }
  };
  
  typedef std::unordered_multimap<std::reference_wrapper<const System::Spec>, RomEntry> RomMap;
  typedef RomMap::iterator RomIterator;
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
    
      RomIteratorRange getRomsForSystem(const System::Spec& spec) { return roms.equal_range(std::reference_wrapper<const System::Spec>(spec)); }
  };
  
}

#endif
