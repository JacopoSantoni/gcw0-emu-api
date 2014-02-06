#include "rom_collection.h"

#include "manager.h"

using namespace std;
using namespace gcw;

void RomCollection::scan()
{
  paths = {"/Users/jack/Documents/Dev/github/gcw0-emu-api/xcode/gba", "/Users/jack/Documents/Dev/github/gcw0-emu-api/xcode/snes"};
  specs = {ConsoleSpec("uncat", "Uncategorized", {}), ConsoleSpec("snes", "Super Nintendo", {"smc","fig"}), ConsoleSpec("gba", "GameBoy Advance", {"gba"}), };
  
  unordered_map<std::string, ConsoleSpec*> extsMapToConsole;
  unordered_set<std::string> exts;
  for (auto &spec : specs)
    for (auto &ext : spec.extensions)
    {
      exts.insert(ext);
      
      if (extsMapToConsole.find(ext) == extsMapToConsole.end())
        extsMapToConsole[ext] = &spec;
      else
        extsMapToConsole[ext] = &specs[0];
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
      rom.console = extsMapToConsole[rom.ext];
      rom.path = &path;
      
      roms.insert(std::pair<ConsoleSpec*,RomEntry>(rom.console,rom));
      
    }
  }
  
  RomMap::iterator it;
  
  for (ConsoleSpec& spec : specs)
  {
    pair<RomMap::iterator, RomMap::iterator> it = roms.equal_range(&spec);
    
    cout << "Console: " << spec.name << "(" << spec.ident << ")" << endl;
    
    for (RomMap::iterator iit = it.first; iit != it.second; ++iit)
    {
      RomEntry &entry = iit->second;
      cout << "\t" << entry.name << " " << entry.ext << " : " << *entry.path << endl;
    }
  }
}