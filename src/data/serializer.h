#ifndef __SERIALIZER_H__
#define __SERIALIZER_H__

#include <vector>

#include "../common/utils.h"

#include "json/writer.h"


using namespace gcw;
using namespace rapidjson;
using namespace std;

class Serializer
{
public:
  template<typename T, typename Writer>
  void serialize(const std::vector<T>& data, Writer& writer) const
  {
    writer.StartArray();
    for (const auto& element : data)
      serialize(element, writer);
    writer.EndArray();
  }
  
  template<typename Writer>
  void serialize(const Path& path, Writer& writer) const
  {
    writer.String(path.c_str(), path.value().length());
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