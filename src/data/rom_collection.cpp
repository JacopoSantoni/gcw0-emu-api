#include "rom_collection.h"

#include "../core/manager.h"

using namespace std;
using namespace gcw;

void RomCollection::scan()
{
  vector<Path> paths = manager->getPersistence()->getRomPaths();
  
  specs = {
    SystemSpec("uncat", "Uncategorized", {}),
    SystemSpec("snes", "Super Nintendo", {"smc","fig"}),
    SystemSpec("gba", "GameBoy Advance", {"gba"})
  };
  
  unordered_map<string, reference_wrapper<const SystemSpec>> extsMapToSystem;
  unordered_set<string> exts;
  for (const SystemSpec& spec : specs)
    for (const auto& ext : spec.extensions)
    {
      exts.insert(ext);
      
      if (extsMapToSystem.find(ext) == extsMapToSystem.end())
        extsMapToSystem.insert(make_pair(ext, reference_wrapper<const SystemSpec>(spec)));
      else
        extsMapToSystem.insert(make_pair(ext, reference_wrapper<const SystemSpec>(specs[0])));
    }
  
  //manager->getLoader()->allowedFileTypes();
  
  
  for (const auto &path : paths)
  {
    vector<string> files = path.findFiles(exts, true);
    
    //Rom
    
    for (auto &file : files)
    {
      
      size_t dot = file.find_last_of(".");
      string ext = file.substr(dot+1, string::npos);
      RomEntry rom = RomEntry(file.substr(0,dot), ext, extsMapToSystem.find(ext)->second, path);
      
      roms.insert(pair<reference_wrapper<const SystemSpec>,RomEntry>(rom.system,rom));
    }
  }
  
  RomMap::iterator it;
  
  for (const SystemSpec& spec : specs)
  {
    pair<RomIterator, RomIterator> it = roms.equal_range(reference_wrapper<const SystemSpec>(spec));
    
    cout << "System: " << spec.name << "(" << spec.ident << ")" << endl;
    
    for (RomIterator iit = it.first; iit != it.second; ++iit)
    {
      RomEntry &entry = iit->second;
      cout << "\t" << entry.name << " " << entry.ext << " : " << entry.path.value() << endl;
    }
  }
}