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
    vector<Path> files = path.findFiles(exts, true);
    
    //Rom
    
    for (auto &file : files)
    {
      std::string name = file.plainName();
      std::string extension = file.extension();

      RomEntry rom = RomEntry(name, extension, extsMapToSystem.find(extension)->second, path);
      
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