#ifndef __SERIALIZER_H__
#define __SERIALIZER_H__

#include <vector>

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
    writer.String("name");
    serialize(details.name, writer);
    writer.String("author");
    serialize(details.author, writer);
    writer.String("porter");
    serialize(details.porter, writer);
    writer.String("webpage");
    serialize(details.webpage, writer);
  }
  
  template<typename Writer>
  void serialize(const CorePreferences& preferences, Writer& writer) const
  {
    writer.String("scaler");
    serialize(preferences.scaler, writer);
  }
  
  template<typename Writer>
  void serialize(const GfxBufferSpec& gfxSpec, Writer& writer) const
  {
    writer.String("width");
    writer.Uint(gfxSpec.width);
    writer.String("height");
    writer.Uint(gfxSpec.height);
    writer.String("format");
    
    const char* formatStr = nullptr;
    
    switch (gfxSpec.format)
    {
      case FORMAT_RGB565: formatStr = "RGB565"; break;
      case FORMAT_RGBA5551: formatStr = "RGB551"; break;
      case FORMAT_RGBA8888: formatStr = "RGBA8888"; break;
      case FORMAT_XRGB888: formatStr = "XRGB888"; break;
    }
    
    writer.String(formatStr);
  }
  
  template<typename Writer>
  void serialize(const ButtonSetting& button, Writer& writer) const
  {
    writer.StartObject();
    writer.String("name");
    serialize(button.getName(), writer);
    writer.String("shiftAmount");
    writer.Uint(button.getShiftAmount());
    writer.String("mask");
    writer.Uint(button.getMask());
    writer.String("defaultMask");
    writer.Uint(button.getDefaultMask());
    writer.String("isRebindable");
    writer.Bool(button.isRebindable());
    writer.String("canBeMultikey");
    writer.Bool(button.canBeMultikey());
    
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
  T unserialize(const Value& value)
  {
    
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