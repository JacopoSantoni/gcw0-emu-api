#ifndef _LOADER_H_
#define _LOADER_H_

#include <iostream>
#include <vector>
#include <string>
#include <map>

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

class Loader
{
  private:
    std::vector<CoreHandle*> cores;
    CoreInterface *core;
    std::map<std::string,std::vector<CoreHandle*> > handledFileTypes;
  
    void loadCoreInfo(CoreHandle *handle, CoreInterface *info);


	public:
    Loader() : core(nullptr) { }

    void scan();
  
    CoreHandle* coreForFile(std::string fileName);
  
    void loadCore(std::string ident);
    void unload();
  
    CoreInterface *getCore() { return core; }
};
  
}

#endif
