#ifndef _LOADER_H_
#define _LOADER_H_

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>

#include "../common/utils.h"
#include "emu_interface.h"

//#define _DUMMY_CORE_

namespace gcw {
  
  #ifdef _DUMMY_CORE_
    extern "C" CoreInterface *retrieve1();
    extern "C" CoreInterface *retrieve2();
  #endif

  struct CoreHandle
  {
    const CoreInfo info;
    void* handle;
    Path path;
    CoreInterface* core;
    
    CoreHandle(const Path& path, const CoreInfo& info) :
    info(info),
    handle(nullptr),
    path(path),
    core(nullptr)
    {
      
    }
    
    void load();
    void unload();
    bool isLoaded() const { return core != nullptr; }
  };

  class Manager;
  class Path;
    
  class Loader
  {
  private:
    Manager* const manager;
    std::vector<CoreHandle> cores;
    std::map<std::string,std::vector<std::reference_wrapper<const CoreHandle>> > handledFileTypes;

    void loadCoreInfo(const CoreHandle& handle, CoreInterface *info);
    void addCoreManagedExtensions(const CoreHandle& handle);


  public:
    Loader(Manager *manager) : manager(manager) { }

    void scan();

    CoreInterface* loadCore(const std::string& ident, const std::string& version) { return loadCore(CoreIdentifier(ident, version)); }
    CoreInterface* loadCore(const CoreIdentifier& ident) {
      auto it = find_if(cores.begin(), cores.end(), [&](const CoreHandle& handle) { return handle.info.ident == ident; });
      return loadCore(*it);
    }
    CoreInterface* loadCore(CoreHandle& handle);

    void unload(CoreInterface* core);

    std::vector<CoreHandle>& getCores() { return cores; }
    
    const std::vector<std::reference_wrapper<CoreHandle>> findCoresForSystem(const System::Type system);
    
    std::optional<CoreHandle&> findCachedCore(const Path& filename, time_t timestamp);
    
    /*std::vector<std::string> allowedFileTypes()
    {
      std::vector<std::string> extensions;
      std::transform(handledFileTypes.begin(), handledFileTypes.end(), std::back_inserter(extensions),
        [](const std::map<std::string,std::vector<std::reference_wrapper<const CoreHandle>>>::value_type &pair){ return pair.first; });
      
      return extensions;
    }*/
  };
  
}

#endif
