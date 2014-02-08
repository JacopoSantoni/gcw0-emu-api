#ifndef _SETTINGS_H_
#define _SETTINGS_H_

#include <string>
#include <vector>

#include "defines.h"

namespace gcw
{
  
enum SettingType : u8
{
  SETTING_BOOL,
  SETTING_PATH,
  SETTING_ENUM
};

class Setting
{
  private:
    SettingType type;
    std::string name;
    std::string ident;
  
  public:
    Setting(SettingType type, std::string name, std::string ident) : type(type), name(name), ident(ident) { }
    std::string getName() { return name; }
    std::string getIdent() { return ident; }
    SettingType getType() { return type; }
};

template<typename V>
class ConcreteSetting : public Setting
{
  private:
    V value;
  
  public:
    ConcreteSetting(SettingType type, std::string name, std::string ident, V value) : Setting(type, name, ident), value(value) { }
    V getValue() { return value; }
    void setValue(V value) { this->value = value; }
};

class BoolSetting : public ConcreteSetting<bool>
{
  
  public:
    BoolSetting(std::string name, std::string ident, bool defaultValue) : ConcreteSetting(SETTING_BOOL, name, ident, defaultValue) { }
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
    EnumSetting(std::string name, std::string ident, EnumSet<T> values, EnumValue<T> value) : ConcreteSetting<EnumValue<T> >(SETTING_ENUM, name, ident, value), values(values)  { }
};
  

struct ButtonSetting
{
  std::string name;
  GCWKey key;
  u8 shiftAmount;
  bool canBeRebound;

  ButtonSetting(std::string name, GCWKey key, u8 shiftAmount, bool canBeRebound) : name(name), key(key), shiftAmount(shiftAmount), canBeRebound(canBeRebound) { }
};

}

#endif