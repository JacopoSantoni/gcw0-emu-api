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
  extern "C" CoreInterface *retrieve();
#endif

struct CoreHandle
{
  const CoreInfo info;
  void *handle;
  std::string fileName;
  CoreInterface *core;
  
	CoreHandle(std::string fileName, CoreInfo info) :
  info(info),
  handle(nullptr),
  fileName(fileName),
  core(nullptr)
	{
    
	}
};

class Manager;
  
class Loader
{
  private:
    Manager* const manager;
    std::vector<CoreHandle*> cores;
    std::map<std::string,std::vector<CoreHandle*> > handledFileTypes;
  
    void loadCoreInfo(CoreHandle *handle, CoreInterface *info);


	public:
    Loader(Manager *manager) : manager(manager) { }

    void scan();
  
    CoreHandle* coreForFile(std::string fileName);
  
    CoreInterface* loadCore(std::string ident);
    void unload();
  
    std::vector<std::string> allowedFileTypes()
    {
      std::vector<std::string> extensions;
      std::transform(handledFileTypes.begin(), handledFileTypes.end(), std::back_inserter(extensions),
        [](const std::map<std::string,std::vector<CoreHandle*> >::value_type &pair){return pair.first;});
      
      return extensions;
    }
};
  
}

#endif
