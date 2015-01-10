#include "persistence.h"

#include "json/document.h"
#include "json/prettywriter.h"
#include "json/filewritestream.h"

#include <fstream>
#include <streambuf>

using namespace gcw;
using namespace std;
using namespace rapidjson;

#ifdef __APPLE__
const Path Persistence::ROOT_PATH = Path("/Users/jack/Documents/Dev/github/gcw0-emu-api/xcode/root");
#else
const Path Persistence::ROOT_PATH = Path("/");
#endif

const Path Persistence::LOADER_PATH = ROOT_PATH + "/usr/local/home/" + ".loader";


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
  
  const Value& jromPaths = doc["rom-paths"];
  for (size_t i = 0; i < jromPaths.Size(); ++i)
    romPaths.push_back(jromPaths[i].GetString());
}

void Persistence::save()
{
  Path settings = LOADER_PATH + "settings.json";
  
  const unique_ptr<char[]> buffer = unique_ptr<char[]>(new char[256]);
  FILE* fp = settings.open(FMode::WRITING);
  
  FileWriteStream stream(fp, buffer.get(), 256);
  
  PrettyWriter<FileWriteStream> writer(stream);
  
  writer.StartObject();
  writer.String("rom-paths");
  writer.StartArray();
  for (const Path& path : romPaths)
    writer.String(path.value().c_str());
  writer.EndArray();
  writer.EndObject();
  
  
  fclose(fp);
  
}
