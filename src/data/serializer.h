#ifndef __SERIALIZER_H__
#define __SERIALIZER_H__

#include <vector>
#include <unordered_map>

#include "../common/utils.h"
#include "../systems/systems.h"

#include "../core/loader.h"
#include "../core/emu_interface.h"

#include "json/writer.h"


using namespace gcw;
using namespace rapidjson;
using namespace std;

class Serializer
{
public:
  template<typename Writer> void serialize(const char* key, bool value, Writer& writer) const { writer.String(key); writer.Bool(value); }
  template<typename Writer> void serialize(const char* key, const char* value, Writer& writer) const { writer.String(key); writer.String(value); }
  template<typename Writer> void serialize(const char* key, u32 value, Writer& writer) const { writer.String(key); writer.Uint(value); }
  template<typename Writer> void serialize(const char* key, const std::string& value, Writer& writer) const { writer.String(key); writer.String(value.c_str()); }

  
  template<typename T, typename Writer>
  void serialize(const std::vector<T>& data, Writer& writer, const char* name = nullptr) const
  {
    if (name)
      writer.String(name);
    
    writer.StartArray();
    for (const auto& element : data)
      serialize(element, writer);
    writer.EndArray();
  }
  
  template<typename V, typename Writer>
  void serialize(const std::unordered_map<std::string,V>& data, Writer& writer, const char* name = nullptr) const
  {
    if (name)
      writer.String(name);
    
    writer.StartObject();
    for (const auto& element : data)
    {
      serialize(element.first, writer);
      serialize(element.second, writer);
    }
    writer.EndObject();
  }
  
  template<typename T, typename Writer>
  void serialize(const T& object, Writer& writer, const char* name) const
  {
    writer.String(name);
    writer.StartObject();
    serialize(object, writer);
    writer.EndObject();
  }
  
  template<typename Writer>
  void serialize(const Path& path, Writer& writer) const
  {
    writer.String(path.c_str(), path.value().length());
  }
  
  template<typename Writer>
  void serialize(const CoreHandle& handle, Writer& writer) const
  {
    writer.StartObject();
    writer.String("path");
    serialize(handle.path, writer);
    serialize(handle.info, writer, "info");
    serialize(handle.preferences, writer, "preferences");
    writer.EndObject();
  }
  
  template<typename Writer>
  void serialize(const CoreInfo& info, Writer& writer) const
  {
    serialize(info.type, writer, "systems");
    writer.String("timestamp");
    writer.Uint(info.timestamp);
    serialize(info.ident, writer, "ident");
    serialize(info.details, writer, "details");
    serialize(info.supportedButtons(), writer, "buttons");
    serialize(info.supportedScalers(), writer, "scalers");
    serialize(info.supportedSettings(), writer, "settings");
    serialize(info.gfxSpec, writer, "gfx");
  }
  
  template<typename Writer>
  void serialize(const CoreIdentifier& ident, Writer& writer) const
  {
    writer.String("name");
    writer.String(ident.ident.c_str());
    writer.String("version");
    writer.String(ident.version.c_str());
  }
  
  template<typename Writer>
  void serialize(const CoreDetails& details, Writer& writer) const
  {
    serialize("name", details.name, writer);
    serialize("author", details.author, writer);
    serialize("porter", details.porter, writer);
    serialize("webpage", details.webpage, writer);
  }
  
  template<typename Writer>
  void serialize(const CorePreferences& preferences, Writer& writer) const
  {
    writer.String("scaler");
    serialize(preferences.scaler, writer);
    serialize(preferences.settings, writer, "settings");
  }
  
  template<typename Writer>
  void serialize(const Setting& setting, Writer& writer) const
  {
    const char* type = nullptr;
    const char* group = nullptr;
    
    switch (setting.type) {
      case Setting::Type::BOOLEAN: type = "boolean"; break;
      case Setting::Type::ENUMERATION: type = "enumeration"; break;
      case Setting::Type::PATH: type = "path"; break;
    }
    
    switch (setting.group) {
      case Setting::Group::VIDEO: group = "video"; break;
      case Setting::Group::AUDIO: group = "audio"; break;
      case Setting::Group::MISC: group = "misc"; break;
    }
    
    writer.StartObject();
    serialize("type", type, writer);
    serialize("group", group, writer);
    serialize("ident", setting.ident, writer);
    serialize("name", setting.name, writer);
    serialize("default", setting.defaultValue, writer);
    serialize("canBeModifiedAtRuntime", setting.canBeModifiedAtRuntime, writer);
    
    if (setting.type == Setting::Type::ENUMERATION)
      serialize(setting.values, writer, "values");
    
    writer.EndObject();
  }
  
  template<typename Writer>
  void serialize(const GfxBufferSpec& gfxSpec, Writer& writer) const
  {
    serialize("width", (u32)gfxSpec.width, writer);
    serialize("height", (u32)gfxSpec.height, writer);
    
    writer.String("format");
    
    const char* formatStr = nullptr;
    
    switch (gfxSpec.format)
    {
      case FORMAT_RGB565: formatStr = "RGB565"; break;
      case FORMAT_RGBA5551: formatStr = "RGB551"; break;
      case FORMAT_RGBA8888: formatStr = "RGBA8888"; break;
      case FORMAT_XRGB888: formatStr = "XRGB888"; break;
    }
    
    serialize("format", formatStr, writer);
  }
  
  template<typename Writer>
  void serialize(const ButtonSetting& button, Writer& writer) const
  {
    writer.StartObject();
    serialize("name", button.getName(), writer);
    serialize("shiftAmount", (u32)button.getShiftAmount(), writer);
    serialize("mask", button.getMask(), writer);
    serialize("defaultMask", button.getDefaultMask(), writer);
    serialize("isRebindable", button.isRebindable(), writer);
    serialize("canBeMultikey", button.canBeMultikey(), writer);
    writer.EndObject();
  }
  
  template<typename Writer>
  void serialize(const System::Type& type, Writer& writer) const {
    const System::Spec& spec = System::getSpecForSystem(type);
    serialize(spec.ident, writer);
  }
  
  template<typename Writer>
  void serialize(const std::string& string, Writer& writer) const {
    writer.String(string.c_str());
  }
};

class Unserializer
{
public:
  template<typename T>
  void unserialize(std::vector<T>& data, const Value& value)
  {
    for (SizeType st = 0; st < value.Size(); ++st)
      data.push_back(unserialize<T>(value[st]));
  }
  
  template<typename T>
  void unserialize(std::unordered_map<std::string,T>& data, const Value& value)
  {
    for (auto it = value.MemberBegin(); it != value.MemberEnd(); ++it)
      data.insert(std::make_pair(it->name.GetString(), it->value.GetString()));
  }
  
  template<typename T>
  T unserialize(const Value& value)
  {
    static_assert(sizeof(T) != sizeof(T), "Unserializer must be specialized");
  }
  

};

template<>
Path Unserializer::unserialize(const Value& value) { return Path(value.GetString()); }

template<>
ButtonSetting Unserializer::unserialize(const Value& value)
{
  std::string name = value["name"].GetString();
  u8 shiftAmount = value["shiftAmount"].GetUint();
  ButtonStatus mask = value["mask"].GetUint();
  ButtonStatus defaultMask = value["defaultMask"].GetUint();
  bool isRebindable = value["isRebindable"].GetBool();
  bool canBeMultikey = value["canBeMultikey"].GetBool();
  
  return ButtonSetting(name, mask, defaultMask, shiftAmount, isRebindable, canBeMultikey);
}

template<>
Setting Unserializer::unserialize(const Value& value)
{
  std::string type = value["type"].GetString();
  std::string group = value["group"].GetString();

  std::string ident = value["ident"].GetString();
  std::string name = value["name"].GetString();
  std::string defaultValue = value["default"].GetString();
  bool canBeModifiedAtRuntime = value["canBeModifiedAtRuntime"].GetBool();
 
  Setting::Group egroup;
  
  if (group == "video") egroup = Setting::Group::VIDEO;
  else if (group == "audio") egroup = Setting::Group::AUDIO;
  else if (group == "misc") egroup = Setting::Group::MISC;
  
  if (type == "boolean")
    return Setting(ident, name, egroup, defaultValue == "true" ? true : false, canBeModifiedAtRuntime);
  else if (type == "path")
    return Setting(ident, name, egroup, defaultValue, canBeModifiedAtRuntime);
  else
  {
    std::vector<std::string> values;
    unserialize(values, value["values"]);
    return Setting(ident, name, egroup, values, defaultValue, canBeModifiedAtRuntime);
  }
}

template<>
CoreDetails Unserializer::unserialize(const Value& value)
{
  std::string name = value["name"].GetString();
  std::string author = value["author"].GetString();
  std::string porter = value["porter"].GetString();
  std::string webpage = value["webpage"].GetString();
  
  return CoreDetails(name, author, porter, webpage);
}

template<>
CorePreferences Unserializer::unserialize(const Value& value)
{
  std::string scaler = value["scaler"].GetString();
  
  CorePreferences preferences;
  
  preferences.scaler = scaler;
  unserialize(preferences.settings, value["settings"]);
  
  return preferences;
}

template<>
CoreIdentifier Unserializer::unserialize(const Value& value)
{
  std::string ident = value["name"].GetString();
  std::string version = value["version"].GetString();
  
  return CoreIdentifier(ident, version);
}

template<>
System::Type Unserializer::unserialize(const Value& value)
{
  return System::getSpecForIdent(value.GetString()).type;
}

template<>
GfxBufferSpec Unserializer::unserialize(const Value& value)
{
  u16 width = value["width"].GetUint();
  u16 height = value["height"].GetUint();
  
  std::string formatStr = value["format"].GetString();
  GfxBufferFormat format = FORMAT_RGB565;
  
  if (formatStr == "RGBA5551") format = FORMAT_RGBA5551;
  else if (formatStr == "RGBA8888") format = FORMAT_RGBA8888;
  else if (formatStr == "XRGB888") format = FORMAT_XRGB888;

  return {width, height, format};
}

template<>
CoreInfo Unserializer::unserialize(const Value& value)
{
  CoreInfo info;
  
  unserialize(info.type, value["systems"]);
  
  std::vector<ButtonSetting> buttons;
  unserialize(buttons, value["buttons"]);
  info.setButtons(buttons);
  
  std::vector<std::string> scalers;
  unserialize(scalers, value["scalers"]);
  info.setScalers(scalers);
  
  std::vector<Setting> settings;
  unserialize(settings, value["settings"]);
  info.setSettings(settings);
  
  info.timestamp = value["timestamp"].GetUint();
  info.ident = unserialize<CoreIdentifier>(value["ident"]);
  info.details = unserialize<CoreDetails>(value["details"]);
  info.gfxSpec = unserialize<GfxBufferSpec>(value["gfx"]);
  
  return info;
}

template<>
CoreHandle Unserializer::unserialize(const Value& value)
{
  CoreInfo info = unserialize<CoreInfo>(value["info"]);
  Path path = unserialize<Path>(value["path"]);
  CorePreferences preferences = unserialize<CorePreferences>(value["preferences"]);
  
  return CoreHandle(path, info, preferences);
}

template<>
std::string Unserializer::unserialize(const Value &value) { return value.GetString(); }


#endif