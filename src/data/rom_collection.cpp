#include "rom_collection.h"

#include "../core/manager.h"

using namespace std;
using namespace gcw;

void RomCollection::scan()
{
  vector<Path> paths = manager->getPersistence()->getRomPaths();

  unordered_map<string, reference_wrapper<const System::Spec>> extsMapToSystem;
  unordered_set<string> exts;
  
  for (auto it = System::getIterator(); System::hasNext(it); ++it)
    for (const auto& ext : it->extensions)
    {
      exts.insert(ext);
      
      if (extsMapToSystem.find(ext) == extsMapToSystem.end())
        extsMapToSystem.insert(make_pair(ext, reference_wrapper<const System::Spec>(*it)));
      else
        extsMapToSystem.insert(make_pair(ext, reference_wrapper<const System::Spec>(System::getSystems()[0])));
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
      
      roms.insert(pair<reference_wrapper<const System::Spec>,RomEntry>(rom.system,rom));
    }
  }
  
  RomMap::iterator it;
  
  for (auto spec = System::getIterator(); System::hasNext(spec); ++spec)
  {
    pair<RomIterator, RomIterator> it = roms.equal_range(reference_wrapper<const System::Spec>(*spec));
    
    cout << "System: " << spec->name << "(" << spec->ident << ")" << endl;
    
    for (RomIterator iit = it.first; iit != it.second; ++iit)
    {
      RomEntry &entry = iit->second;
      cout << "\t" << entry.name << " " << entry.ext << " : " << entry.path.value() << endl;
    }
  }
}