#include "loader.h"

#include "dlfcn.h"
#include "utils.h"

const char* CORES_PATH = "cores/";
const char* LIBRARY_EXTENSION = "dylib";

using namespace std;
using namespace gcw;

void Loader::loadCoreInfo(CoreHandle *handle, CoreInterface *info)
{
  cores.push_back(handle);
  vector<string> extensions = info->supportedExtensions();
  for (string ext : extensions)
    handledFileTypes[ext].push_back(handle);
  
  LOG("Found core: %s ident: %s\n",handle->info.name.c_str(), handle->info.ident.c_str());
}

void Loader::scan()
{
  #ifdef _DUMMY_CORE_
    CoreInterface *core = retrieve();
    CoreHandle *handle = new CoreHandle("dummy", core->info());
    loadCoreInfo(handle, core);
    return;
  #else  
    vector<string> files = Files::findFiles(CORES_PATH,LIBRARY_EXTENSION);
    
    LOG("Cores folder: %s\n",CORES_PATH);


    for (string file : files)
    {    
      void *handle = dlopen((CORES_PATH+file).c_str(), RTLD_LOCAL|RTLD_NOW);
      CoreInterface* (*retrieve)();
      *(void**) (&retrieve) = dlsym(handle, "retrieve");
      
      if (retrieve)
      {    
        CoreInterface *interface = retrieve();
        
        // we create the CoreHandle but without setting it's pointer to the CoreInterface or to the retrieve
        // function since we're just scanning 
        CoreHandle *coreHandle = new CoreHandle(CORES_PATH+file, interface->info());
        loadCoreInfo(coreHandle, interface);
        
      }
      
      dlclose(handle);
    }
  #endif
}

void Loader::unload()
{
  #ifndef _DUMMY_CORE_  
    if (core)
    {
      vector<CoreHandle*>::iterator it = find_if(cores.begin(), cores.end(), [&](const CoreHandle* handle) { return handle->core == core; });
      
      if (it != cores.end())
      {
        CoreHandle *handle = *it;
        LOG("Unloading core: %s\n", handle->info.ident.c_str());
        handle->core = nullptr;
        dlclose(handle->handle);
        handle->handle = nullptr;
        core = nullptr;
      }
    }
  #endif
}

void Loader::loadCore(std::string ident)
{  
  vector<CoreHandle*>::iterator it = find_if(cores.begin(), cores.end(), [&](const CoreHandle* handle) { return handle->info.ident == ident; });

  if (it != cores.end()/* && core != (*it)->core*/)
  {
    unload();
    CoreHandle *handle = *it;
    
    LOG("Loading core %s at %s\n",handle->info.ident.c_str(),handle->fileName.c_str());
    
    #ifdef _DUMMY_CORE_
      handle->core = retrieve();
      core = handle->core;
    #else
      void *dlhandle = dlopen(handle->fileName.c_str(), RTLD_LOCAL|RTLD_NOW);
      CoreInterface* (*retrieve)();
      *(void**) (&retrieve) = dlsym(dlhandle, "retrieve");
      
      handle->handle = dlhandle;
      handle->core = retrieve();
      core = handle->core;
    #endif
  }
}


CoreHandle* Loader::coreForFile(std::string fileName)
{
  LOG("Searching for a core to load %s\n", fileName.c_str());
  
  std::string::size_type idx;
  idx = fileName.rfind('.');
  
  if(idx != std::string::npos)
  {
    std::string extension = fileName.substr(idx+1);
  
    map<string, vector<CoreHandle*> >::iterator it = handledFileTypes.find(extension);
    
    if (it != handledFileTypes.end())
    {
      CoreHandle *handle = it->second[0];
      LOG("Found core %s that manages %s files\n", handle->info.name.c_str(), extension.c_str());
      return handle;
    }
    else
    {
      LOG("No core found to load %s files!\n", extension.c_str());
    }
  }
  
  return nullptr;
}

