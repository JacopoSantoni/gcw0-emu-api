#ifndef _SETTINGS_H_
#define _SETTINGS_H_

#include <string>
#include <vector>

#include "defines.h"

namespace gcw
{

class Setting
{
  private:
    std::string name;
    std::string ident;
  
  public:
    Setting(std::string name, std::string ident) : name(name), ident(ident) { }
    std::string getName() { return name; }
    std::string getIdent() { return ident; }
};

template<typename V>
class ConcreteSetting : public Setting
{
  private:
    V defaultValue;
  
  public:
    ConcreteSetting(std::string name, std::string ident, V defaultValue) : Setting(name, ident), defaultValue(defaultValue) { }
    V getDefaultValue() { return defaultValue; }
};

class BoolSetting : public ConcreteSetting<bool>
{
  
  public:
    BoolSetting(std::string name, std::string ident, bool defaultValue) : ConcreteSetting(name, ident, defaultValue) { }
};

template<typename T>
class EnumValue
{
  private:
    const std::string name;
    T value;
  
  public:
    EnumValue(std::string name, T value) : name(name), value(value) { }
};

template<typename T> using EnumSet = std::vector< EnumValue<T> >;

template <typename T>
class EnumSetting : public ConcreteSetting<EnumValue<T> >
{
  private:
    EnumSet<T> values;
  
  public:
    EnumSetting(std::string name, std::string ident, EnumSet<T> values, EnumValue<T> defaultValue) : ConcreteSetting<EnumValue<T> >(name, ident, defaultValue), values(values)  { }
};
  

struct ButtonSetting
{
  std::string name;
  GCWKey key;
  u8 shiftAmount;
  bool canBeRebound;
  
  ButtonSetting(std::string name, GCWKey key, u8 shiftAmount, bool canBeRebound) : name(name), key(key), shiftAmount(shiftAmount), canBeRebound(canBeRebound) { }
};

struct AnalogSetting
{
  std::string name;
  float activeZone[2][2];
  
  AnalogSetting(std::string name, float activeZone) { }
};

  
}


#endif