#include "rom_collection.h"

#include "manager.h"

using namespace std;
using namespace gcw;

void RomCollection::scan()
{
  paths = {"/Users/jack/Documents/Dev/github/gcw0-emu-api/xcode/gba", "/Users/jack/Documents/Dev/github/gcw0-emu-api/xcode/snes"};
  specs = {SystemSpec("uncat", "Uncategorized", {}), SystemSpec("snes", "Super Nintendo", {"smc","fig"}), SystemSpec("gba", "GameBoy Advance", {"gba"}), };
  
  unordered_map<std::string, SystemSpec*> extsMapToSystem;
  unordered_set<std::string> exts;
  for (auto &spec : specs)
    for (auto &ext : spec.extensions)
    {
      exts.insert(ext);
      
      if (extsMapToSystem.find(ext) == extsMapToSystem.end())
        extsMapToSystem[ext] = &spec;
      else
        extsMapToSystem[ext] = &specs[0];
    }
  
  //manager->getLoader()->allowedFileTypes();
  
  for (auto &path : paths)
  {
    vector<string> files = Files::findFiles(path, exts, true);
    
    //Rom
    
    for (auto &file : files)
    {
      RomEntry rom;
      
      size_t dot = file.find_last_of(".");
      rom.name = file.substr(0,dot);
      rom.ext = file.substr(dot+1, string::npos);
      rom.system = extsMapToSystem[rom.ext];
      rom.path = &path;
      
      roms.insert(std::pair<SystemSpec*,RomEntry>(rom.system,rom));
      
    }
  }
  
  RomMap::iterator it;
  
  for (SystemSpec& spec : specs)
  {
    pair<RomIterator, RomIterator> it = roms.equal_range(&spec);
    
    cout << "System: " << spec.name << "(" << spec.ident << ")" << endl;
    
    for (RomIterator iit = it.first; iit != it.second; ++iit)
    {
      RomEntry &entry = iit->second;
      cout << "\t" << entry.name << " " << entry.ext << " : " << *entry.path << endl;
    }
  }
}