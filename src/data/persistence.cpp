#include "persistence.h"

using namespace gcw;

#ifdef __APPLE__
const Path Persistence::ROOT_PATH = Path("/Users/jack/Documents/Dev/github/gcw0-emu-api/xcode/root");
#else
const Path Persistence::ROOT_PATH = Path("/");
#endif


Keybind Persistence::keyBindOveriddenFor(const CoreIdentifier& identifier, const std::string& keyName)
{
  KeybindIteratorRange keys = keybinds.equal_range(identifier);
  
  KeybindIterator it = std::find_if(keys.first, keys.second, [&](const KeybindMap::value_type& kb) { return kb.second.name == keyName; });
  
  if (it != keybinds.end())
    return it->second;
  else
    return Keybind();
}