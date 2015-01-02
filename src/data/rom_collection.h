#ifndef _ROM_LIST_H_
#define _ROM_LIST_H_

#include <vector>
#include <string>
#include <unordered_set>
#include <unordered_map>

#include "../common/utils.h"
#include "../common/defines.h"

namespace std {
  template<typename T>
  struct hash<reference_wrapper<T>>
  {
    std::size_t operator()(const std::reference_wrapper<T>& k) const
    {
      return hash<T*>()(&k.get());
    }
  };
}

namespace std {
template<typename T>
  bool operator==(const std::reference_wrapper<T>& r1, const std::reference_wrapper<T>& r2)
  {
    return &(r1.get()) == &(r2.get());
  }
}

namespace gcw {
  
  
  struct SystemSpec
  {
    std::string ident;
    std::string name;
    std::vector<std::string> extensions;
    
    SystemSpec(std::string&& ident, std::string &&name, std::vector<std::string> &&extensions) : ident(ident), name(name), extensions(extensions) { }
  };
  
  struct RomEntry
  {
    const std::string name;
    const std::string ext;
    const SystemSpec& system;
    const Path path;
    
    RomEntry(const std::string& name, const std::string& ext, const SystemSpec& system, const Path path) : name(name), ext(ext), system(system), path(path) { }
  };
  
  typedef std::unordered_multimap<std::reference_wrapper<const SystemSpec>, RomEntry> RomMap;
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
    
      const std::vector<SystemSpec>& getSystems() { return specs; }
      RomIteratorRange getRomsForSystem(const SystemSpec& spec) { return roms.equal_range(std::reference_wrapper<const SystemSpec>(spec)); }
  };
  
}

#endif
