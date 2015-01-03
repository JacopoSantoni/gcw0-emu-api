#ifndef _LOADER_H_
#define _LOADER_H_

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>

#include "emu_interface.h"

#define _DUMMY_CORE_

namespace gcw {
  
#ifdef _DUMMY_CORE_
  extern "C" CoreInterface *retrieve1();
  extern "C" CoreInterface *retrieve2();
#endif

struct CoreHandle
{
  const CoreInfo info;
  void* handle;
  std::string fileName;
  CoreInterface* core;
  
	CoreHandle(std::string fileName, CoreInfo info) :
  info(info),
  handle(nullptr),
  fileName(fileName),
  core(nullptr)
	{
    
	}
  
  void load();
  void unload();
  bool isLoaded() const { return core != nullptr; }
};

class Manager;
  
class Loader
{
  private:
    Manager* const manager;
    std::vector<CoreHandle> cores;
    std::map<std::string,std::vector<std::reference_wrapper<const CoreHandle>> > handledFileTypes;
  
    void loadCoreInfo(const CoreHandle& handle, CoreInterface *info);


	public:
    Loader(Manager *manager) : manager(manager) { }

    void scan();
  
    CoreInterface* loadCore(const std::string& ident, const std::string& version) { return loadCore(CoreIdentifier(ident, version)); }
    CoreInterface* loadCore(const CoreIdentifier& ident);
    void unload(CoreInterface* core);
  
    const std::vector<CoreHandle>& getCores() { return cores; }
  
    std::vector<std::string> allowedFileTypes()
    {
      std::vector<std::string> extensions;
      std::transform(handledFileTypes.begin(), handledFileTypes.end(), std::back_inserter(extensions),
        [](const std::map<std::string,std::vector<std::reference_wrapper<const CoreHandle>>>::value_type &pair){ return pair.first; });
      
      return extensions;
    }
};
  
}

#endif
