#include "persistence.h"

#include "json/document.h"
#include "json/prettywriter.h"
#include "json/filewritestream.h"

#include "../core/manager.h"

#include "serializer.h"

#include <fstream>
#include <streambuf>
#include <memory>

using namespace gcw;
using namespace std;
using namespace rapidjson;

#ifdef __APPLE__
const Path Persistence::ROOT_PATH = Path("/Users/jack/Documents/Dev/github/gcw0-emu-api/xcode/root");
const Path Persistence::HOME_PATH = ROOT_PATH + "/usr/local/home";
const Path Persistence::LOADER_PATH = HOME_PATH + "loader";

const string Persistence::CORES_EXTENSION = "dylib";

#else
const Path Persistence::ROOT_PATH = Path("/");
const Path Persistence::HOME_PATH = "/usr/local/home";
const Path Persistence::LOADER_PATH = ROOT_PATH + HOME_PATH + ".loader";

const string Persistence::CORES_EXTENSION = "so";

#endif

const Path Persistence::CORES_PATH = LOADER_PATH + "cores";
const Path Persistence::SAVES_PATH = LOADER_PATH + "data" + "save";
const Path Persistence::SRAM_PATH = LOADER_PATH + "data" + "sram";


Path Persistence::pathFor(PathType type)
{
  switch (type) {
    case PathType::ROOT: return ROOT_PATH;
    case PathType::HOME: return HOME_PATH;
    case PathType::LOADER: return LOADER_PATH;
    case PathType::CORES: return CORES_PATH;
    case PathType::SAVES: return SAVES_PATH;
    case PathType::SRAM: return SRAM_PATH;
      
    case PathType::SETTINGS_FILE: return LOADER_PATH + "settings.json";
    case PathType::CORE_CACHE: return LOADER_PATH + "corecache.json";
    
    default: return Path();
  }
}

const std::string& Persistence::coreExtension() { return CORES_EXTENSION; }

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

void Persistence::createFolderStructure()
{
  Files::createFolder(LOADER_PATH);
  Files::createFolder(CORES_PATH);
  Files::createFolder(SAVES_PATH);
  Files::createFolder(SRAM_PATH);
}

Path Persistence::savePath(const CoreInfo& core, const gcw::RomEntry *entry, SaveSlot slot)
{
  Path path = SAVES_PATH + (core.ident.ident + (core.type.size() > 1 ? (string("-") + entry->system.ident) : "")) + (entry->name + "-" + Text::to_string(slot) + ".sav");
  Files::createFolder(path.folder());
  return path;
}

Path Persistence::sramPath(const CoreInfo &core, const RomEntry *entry)
{
  Path path = SRAM_PATH + (core.ident.ident + (core.type.size() > 1 ? (string("-") + entry->system.ident) : ""));
  Files::createFolder(path);
  return path;
}

void Persistence::load()
{
  Path settings = LOADER_PATH + "settings.json";
  
  if (!settings.exists())
    return;

  Document doc;
  
  ifstream file(settings.value());
  string buffer;
  
  file.seekg(ios::end);
  buffer.reserve(file.tellg());
  file.seekg(ios::beg);
  
  buffer.assign((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

  doc.Parse(buffer.c_str());

  Unserializer unserializer;
  
  unserializer.unserialize(romPaths, doc["rom-paths"]);
  
  unserializer.unserialize(manager->getLoader()->getCores(), doc["core-cache"]);
}

void Persistence::save()
{
  Path settings = pathFor(PathType::SETTINGS_FILE);
  
  const unique_ptr<char[]> buffer = unique_ptr<char[]>(new char[256]);
  FILE* fp = settings.open(FMode::WRITING);
  
  FileWriteStream stream(fp, buffer.get(), 256);
  
  PrettyWriter<FileWriteStream> writer(stream);
  
  Serializer serializer;
  
  writer.StartObject();
  
  serializer.serialize(romPaths, writer, "rom-paths");
  
  const vector<CoreHandle>& cores = manager->getLoader()->getCores();
  serializer.serialize(cores, writer, "core-cache");
  
  writer.EndObject();
  
  fclose(fp);
}
