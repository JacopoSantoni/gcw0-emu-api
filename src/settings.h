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
    V& getValue() { return value; }
    void setValue(V value) { this->value = value; }
};

class BoolSetting : public ConcreteSetting<bool>
{
  
  public:
    BoolSetting(std::string name, std::string ident, bool value) : ConcreteSetting(SETTING_BOOL, name, ident, value) { }
};
  
class PathSetting : public ConcreteSetting<std::string>
{
  public:
    PathSetting(std::string name, std::string ident, std::string value) : ConcreteSetting(SETTING_PATH, name, ident, value) { }
};

  
class EnumValue
{
  private:
    const std::string name;
    
  public:
    EnumValue(std::string name) : name(name) { }
    const std::string& getName() { return name; }
};
  
template<typename T>
class ConcreteEnumValue : public EnumValue
{
  private:
    T value;
    
  public:
    ConcreteEnumValue(std::string name, T value) : EnumValue(name), value(value) { }
};

//template<typename T> using EnumSet = std::vector< EnumValue<T> >;
  
typedef std::vector<EnumValue*> EnumSet;
  
/*class EnumSetting
{
  private:
  
  public:
    virtual size_t count() = 0;

};*/

class EnumSetting : public ConcreteSetting<EnumValue*>
{
  private:
    EnumSet values;
  
  public:
    EnumSetting(std::string name, std::string ident, EnumSet values, EnumValue* value) : ConcreteSetting<EnumValue*>(SETTING_ENUM, name, ident, value), values(values)  { }
  
    void next() {
      EnumValue *current = getValue();
      if (current == values.back())
        setValue(values.front());
      else
      {
        EnumSet::iterator it = find(values.begin(), values.end(), current);
        setValue(*(++it));
      }
    }
  
    void prev() {
      EnumValue *current = getValue();
      if (current == values.front())
        setValue(values.back());
      else
      {
        EnumSet::iterator it = find(values.begin(), values.end(), current);
        setValue(*(--it));
      }
    }
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