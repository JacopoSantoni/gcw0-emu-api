#include "persistence.h"

using namespace gcw;
using namespace std;

#ifdef __APPLE__
const Path Persistence::ROOT_PATH = Path("/Users/jack/Documents/Dev/github/gcw0-emu-api/xcode/root");
#else
const Path Persistence::ROOT_PATH = Path("/");
#endif


optional<const Keybind&> Persistence::keyBindOveriddenFor(const CoreIdentifier& identifier, const std::string& keyName)
{
  /*if (keybinds.empty())
  {
    keybinds.insert(make_pair(CoreIdentifier("geekboy", "0.2a"), Keybind("Left", GCW_KEY_RIGHT)));
    keybinds.insert(make_pair(CoreIdentifier("geekboy", "0.2a"), Keybind("Right", GCW_KEY_LEFT)));
  }*/
  
  
  KeybindIteratorRange keys = keybinds.equal_range(identifier);
  
  KeybindIterator it = std::find_if(keys.first, keys.second, [&](const KeybindMap::value_type& kb) { return kb.second.name == keyName; });
  
  if (it != keybinds.end())
    return optional<const Keybind&>(it->second);
  else
    return std::nullopt;
}

void Persistence::setKeybind(const CoreIdentifier& identifier, const string& name, GCWKey key)
{
  KeybindMap::iterator it = std::find_if(keybinds.begin(), keybinds.end(), [&](KeybindMap::value_type& pair) { return pair.second.name == name && pair.first == identifier; });
  
  if (it != keybinds.end())
    it->second.key = key;
  else
    keybinds.insert(make_pair(identifier, Keybind(name, key)));
}
