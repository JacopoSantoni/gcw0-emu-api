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
    writer.String(handle.fileName.c_str());
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


#endif