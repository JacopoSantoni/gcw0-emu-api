#include "loader.h"

#include "dlfcn.h"
#include "../common/utils.h"
#include "manager.h"

using namespace std;
using namespace gcw;

void Loader::loadCoreInfo(const CoreHandle& handle, CoreInterface *info)
{
  cores.push_back(handle);
  addCoreManagedExtensions(handle);
  LOG("Found core: %s ident: %s\n",handle.info.details.name.c_str(), handle.info.ident.identifier().c_str());
}

void Loader::addCoreManagedExtensions(const CoreHandle& handle)
{
  for (const System::Type& type : handle.info.type)
  {
    const vector<string>& extensions = System::getSpecForSystem(type).extensions;
    
    for (const string& ext : extensions)
      handledFileTypes[ext].push_back(handle);
  }
}

optional<CoreHandle&> Loader::findCachedCore(const Path& filename, time_t timestamp)
{
  for (CoreHandle& handle : cores)
  {
    if (handle.path == filename /*&& handle.info.timestamp == timestamp*/)
      return optional<CoreHandle&>(handle);
  }
  
  return nullopt;
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
  
  const Path coresFolder = Persistence::pathFor(PathType::CORES);
  
  vector<Path> files = coresFolder.findFiles(Persistence::coreExtension(), false);
    
  LOG("Cores folder: %s\n",coresFolder.c_str());

  for (const Path& file : files)
  {    
    Path coreFile = coresFolder + file.value();
    time_t timestamp = coreFile.timeModified();
    
    optional<CoreHandle&> cachedCore = findCachedCore(coreFile, timestamp);
    
    /* a cached core with same filename and same timestamp has been found, we can skip scanning it and just manage extensions */
    if (cachedCore)
    {
      LOG("Found cached core: %s ident: %s\n", cachedCore->info.details.name.c_str(), cachedCore->info.ident.identifier().c_str());
      addCoreManagedExtensions(*cachedCore);
      continue;
    }

    void *handle = dlopen(coreFile.c_str(), RTLD_LOCAL|RTLD_NOW);
    CoreInterface* (*retrieve)();
    *(void**) (&retrieve) = dlsym(handle, "retrieve");
    
    if (retrieve)
    {    
      CoreInterface *interface = retrieve();
      
      // we create the CoreHandle but without setting it's pointer to the CoreInterface or to the retrieve
      // function since we're just scanning
      CoreInfo info = interface->info();
      info.timestamp = timestamp;
      CoreHandle coreHandle = CoreHandle(coreFile.c_str(), info);
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

  LOG("Loading core %s at %s\n",handle.info.ident.ident.c_str(),handle.path.c_str());
  
  #ifdef _DUMMY_CORE_
    if (handle.info.ident.ident == "dummy1")
      handle.core = retrieve1();
    else
      handle.core = retrieve2();
  
    handle.core->setManager(manager);
    return handle.core;


  #else
    void *dlhandle = dlopen(handle.path.c_str(), RTLD_LOCAL|RTLD_NOW);
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

