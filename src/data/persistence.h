#ifndef _PERSISTENCE_H_
#define _PERSISTENCE_H_

#include "../common/utils.h"

#include <vector>
#include <map>
#include "../common/optional.h"

namespace gcw {
  
  struct RomEntry;
  
  struct Keybind
  {
    std::string name;
    GCWKey key;
    
    Keybind(const std::string&& name, GCWKey key) : name(name), key(key) { }
    Keybind(const std::string& name, GCWKey key) : name(name), key(key) { }
    
    bool isValid() { return !name.empty(); }
  };
    
    typedef std::multimap<CoreIdentifier, Keybind> KeybindMap;
    typedef KeybindMap::const_iterator KeybindIterator;
    typedef std::pair<KeybindIterator, KeybindIterator> KeybindIteratorRange;

  class Persistence
  {
  private:
    std::vector<Path> romPaths;
    Path savesPath;
    
    KeybindMap keybinds;

  public:

    void addRomPath(Path path) { romPaths.push_back(path); }
    const std::vector<Path>& getRomPaths() { return romPaths; }

    void setSavesPath(Path path) { savesPath = path; }
    const Path& getSavesPath() const { return savesPath; }
    
    std::optional<CoreIdentifier> defaultCoreForEntry(const RomEntry& folder) {
      return std::optional<CoreIdentifier>(CoreIdentifier("gambatte", "1.0"));
      //return std::optional<CoreIdentifier>(CoreIdentifier("geekboy", "0.2a"));
    }
    
    std::optional<const Keybind&> keyBindOveriddenFor(const CoreIdentifier& identifier, const std::string& keyName);
    void setKeybind(const CoreIdentifier& identifier, const std::string& name, GCWKey key);
    
    
    const static Path ROOT_PATH;
    const static Path LOADER_PATH;
    
    
    void save();
    void load();

    //   paths = {gba", "/Users/jack/Documents/Dev/github/gcw0-emu-api/xcode/snes"};

  };
  
}

#endif
