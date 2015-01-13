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
CoreInfo Unserializer::unserialize(const Value& value)
{
  CoreInfo info;
  
  unserialize(info.type, value["systems"]);
  std::vector<ButtonSetting> buttons;
  unserialize(buttons, value["buttons"]);
  info.setButtons(buttons);
  info.timestamp = value["timestamp"].GetUint();
  info.ident = unserialize<CoreIdentifier>(value["ident"]);
  info.details = unserialize<CoreDetails>(value["details"]);
  
  return info;
}

template<>
CoreHandle Unserializer::unserialize(const Value& value)
{
  CoreInfo info = unserialize<CoreInfo>(value["info"]);
  Path path = unserialize<Path>(value["path"]);
  
  return CoreHandle(path, info);
}


#endif