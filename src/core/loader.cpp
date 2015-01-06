#include "loader.h"

#include "dlfcn.h"
#include "../common/utils.h"
#include "manager.h"

// TODO gcw vvvv
//const char* CORES_PATH = "cores/";
const char* CORES_PATH = "/Users/jack/Documents/Dev/github/gcw0-emu-api/cores/";
const char* LIBRARY_EXTENSION = "dylib";

using namespace std;
using namespace gcw;

void Loader::loadCoreInfo(const CoreHandle& handle, CoreInterface *info)
{
  cores.push_back(handle);
  
  for (const System::Type& type : handle.info.type)
  {
    const vector<string>& extensions = System::getSpecForSystem(type).extensions;
    
    for (const string& ext : extensions)
      handledFileTypes[ext].push_back(handle);
  }
  
  LOG("Found core: %s ident: %s\n",handle.info.name.c_str(), handle.info.identifier().c_str());
}

void Loader::scan()
{
  #ifdef _DUMMY_CORE_
    CoreInterface *core = retrieve1();
    CoreHandle handle = CoreHandle("dummy1", core->info());
    loadCoreInfo(handle, core);
  
    CoreInterface* core2 = retrieve2();
    CoreHandle handle2 = CoreHandle("dummy2", core2->info());
    loadCoreInfo(handle2, core2);
    return;
  #else  
    vector<Path> files = Files::findFiles(CORES_PATH,LIBRARY_EXTENSION,false);
    
    LOG("Cores folder: %s\n",CORES_PATH);


    for (const Path& file : files)
    {    
      void *handle = dlopen((CORES_PATH+file.value()).c_str(), RTLD_LOCAL|RTLD_NOW);
      CoreInterface* (*retrieve)();
      *(void**) (&retrieve) = dlsym(handle, "retrieve");
      
      if (retrieve)
      {    
        CoreInterface *interface = retrieve();
        
        // we create the CoreHandle but without setting it's pointer to the CoreInterface or to the retrieve
        // function since we're just scanning 
        CoreHandle coreHandle = CoreHandle(CORES_PATH+file.value(), interface->info());
        loadCoreInfo(coreHandle, interface);
      }
      
      dlclose(handle);
    }
  #endif
}

void Loader::unload(CoreInterface* core)
{
  #ifndef _DUMMY_CORE_  
    if (core)
    {
      vector<CoreHandle>::iterator it = find_if(cores.begin(), cores.end(), [&](const CoreHandle& handle) { return handle.core == core; });
      
      if (it != cores.end())
      {
        CoreHandle& handle = *it;
        LOG("Unloading core: %s\n", handle.info.identifier().c_str());
        handle.core = nullptr;
        dlclose(handle.handle);
        handle.handle = nullptr;
        core = nullptr;
      }
    }
  #endif
}

CoreInterface* Loader::loadCore(CoreHandle& handle)
{
  unload(handle.core);

  LOG("Loading core %s at %s\n",handle.info.ident.ident.c_str(),handle.fileName.c_str());
  
  #ifdef _DUMMY_CORE_
    if (handle.info.ident.ident == "dummy1")
      handle.core = retrieve1();
    else
      handle.core = retrieve2();
  
    handle.core->setManager(manager);
    return handle.core;


  #else
    void *dlhandle = dlopen(handle.fileName.c_str(), RTLD_LOCAL|RTLD_NOW);
    CoreInterface* (*retrieve)();
    *(void**) (&retrieve) = dlsym(dlhandle, "retrieve");
    
    handle.handle = dlhandle;
    handle.core = retrieve();
    handle.core->setManager(manager);
    return handle.core;
  #endif
}

const vector<reference_wrapper<CoreHandle>> Loader::findCoresForSystem(const System::Type system)
{
  vector<reference_wrapper<CoreHandle>> scores;
  
  for (CoreHandle& handle : cores)
  {
    if (find(handle.info.type.begin(), handle.info.type.end(), system) != handle.info.type.end())
      scores.push_back(handle);
  }
  
  return scores;
}

/*
const CoreHandle& Loader::coreForFile(std::string fileName)
{
  LOG("Searching for a core to load %s\n", fileName.c_str());
  
  std::string::size_type idx;
  idx = fileName.rfind('.');
  
  if(idx != std::string::npos)
  {
    std::string extension = fileName.substr(idx+1);
  
    map<string, vector<std::reference_wrapper<const CoreHandle>> >::iterator it = handledFileTypes.find(extension);
    
    if (it != handledFileTypes.end())
    {
      const CoreHandle& handle = it->second[0];
      LOG("Found core %s that manages %s files\n", handle.info.name.c_str(), extension.c_str());
      return handle;
    }
    else
    {
      LOG("No core found to load %s files!\n", extension.c_str());
    }
  }
  
  return std::reference_wrapper<const CoreHandle>(CoreHandle());
}*/

