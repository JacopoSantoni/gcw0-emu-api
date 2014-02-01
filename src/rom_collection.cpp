#include "rom_collection.h"

#include "manager.h"

using namespace std;
using namespace gcw;

void RomCollection::scan()
{
  vector<string> exts = manager->getLoader()->allowedFileTypes();
  
  for (auto &path : paths)
  {
    vector<string> files = Files::findFiles(path, exts, true);
  }
}