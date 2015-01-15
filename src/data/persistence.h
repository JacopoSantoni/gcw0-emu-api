#ifndef _PERSISTENCE_H_
#define _PERSISTENCE_H_

#include "../common/utils.h"

#include <vector>
#include <map>
#include "../common/optional.h"

namespace gcw {
  
  struct RomEntry;
  class Manager;
  struct CoreInfo;
  
  enum class PathType
  {
    ROOT,
    HOME,
    LOADER,
    CORES,
    SAVES,
    SRAM,
    SAVES_THUMBNAILS,
    
    ART,
    
    SETTINGS_FILE,
    CORE_CACHE
  };
  
  
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
    Manager* manager;
    std::vector<Path> romPaths;
    Path savesPath;
    
    KeybindMap keybinds;
    
    const static Path ROOT_PATH;
    const static Path HOME_PATH;
    const static Path LOADER_PATH;
    const static Path CORES_PATH;
    const static Path SAVES_PATH;
    const static Path SRAM_PATH;
    const static std::string CORES_EXTENSION;

  public:
    Persistence(Manager* manager) { this->manager = manager; }

    void addRomPath(Path path) { romPaths.push_back(path); }
    const std::vector<Path>& getRomPaths() { return romPaths; }

    std::optional<CoreIdentifier> defaultCoreForEntry(const RomEntry& folder) {
      //return std::optional<CoreIdentifier>(CoreIdentifier("gambatte", "1.0"));
      return std::optional<CoreIdentifier>(CoreIdentifier("geekboy", "0.2a"));
    }
    
    std::optional<const Keybind&> keyBindOveriddenFor(const CoreIdentifier& identifier, const std::string& keyName);
    void setKeybind(const CoreIdentifier& identifier, const std::string& name, GCWKey key);
    
    void save();
    void load();
    void createFolderStructure();
    
    Path savePath(const CoreInfo& core, const RomEntry* entry, SaveSlot slot);
    Path sramPath(const CoreInfo& core, const RomEntry* entry);

    
    static Path pathFor(PathType type);
    static const std::string& coreExtension();
    
    //   paths = {gba", "/Users/jack/Documents/Dev/github/gcw0-emu-api/xcode/snes"};

  };
  
}

#endif
