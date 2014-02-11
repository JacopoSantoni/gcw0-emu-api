#ifndef _PERSISTENCE_H_
#define _PERSISTENCE_H_

#include "utils.h"

#include <vector>

namespace gcw {

class Persistence
{
  private:
    std::vector<Path> romPaths;
  
  
  public:
  
    void addRomPath(Path path) { romPaths.push_back(path); }
    std::vector<Path>& getRomPaths() { return romPaths; }
  
  
    const static Path ROOT_PATH;

  //   paths = {gba", "/Users/jack/Documents/Dev/github/gcw0-emu-api/xcode/snes"};

};
  
}

#endif
