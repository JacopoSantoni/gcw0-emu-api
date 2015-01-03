#ifndef _PERSISTENCE_H_
#define _PERSISTENCE_H_

#include "../common/utils.h"

#include <vector>

namespace gcw {

class Persistence
{
  private:
    std::vector<Path> romPaths;
    Path savesPath;
  
  public:
  
    void addRomPath(Path path) { romPaths.push_back(path); }
    const std::vector<Path>& getRomPaths() { return romPaths; }
  
    void setSavesPath(Path path) { savesPath = path; }
    const Path& getSavesPath() const { return savesPath; }
  
    const static Path ROOT_PATH;

  //   paths = {gba", "/Users/jack/Documents/Dev/github/gcw0-emu-api/xcode/snes"};

};
  
}

#endif
