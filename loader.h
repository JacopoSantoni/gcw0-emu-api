#ifndef _LOADER_H_
#define _LOADER_H_

#include <iostream>
#include <vector>
#include <string>
#include <map>

#include "emu_interface.h"

namespace gcw {

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
