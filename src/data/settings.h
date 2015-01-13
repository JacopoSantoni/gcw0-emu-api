#ifndef _SETTINGS_H_
#define _SETTINGS_H_

#include <string>
#include <vector>
#include <algorithm>

#include "../common/defines.h"
#include "../common/keys.h"

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
    Setting(SettingType type, const std::string& name, const std::string& ident) : type(type), name(name), ident(ident) { }
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
    ConcreteSetting(SettingType type, const std::string& name, const std::string& ident, V value) : Setting(type, name, ident), value(value) { }
    V& getValue() { return value; }
    void setValue(V value) { this->value = value; }
};

class BoolSetting : public ConcreteSetting<bool>
{
  
  public:
    BoolSetting(const std::string& name, const std::string ident, bool value) : ConcreteSetting(SETTING_BOOL, name, ident, value) { }
};
  
class PathSetting : public ConcreteSetting<std::string>
{
  public:
    PathSetting(const std::string& name, const std::string ident, const std::string value) : ConcreteSetting(SETTING_PATH, name, ident, value) { }
};

  
class EnumValue
{
  private:
    const std::string name;
    
  public:
    EnumValue(const std::string& name) : name(name) { }
    const std::string& getName() { return name; }
};
  
template<typename T>
class ConcreteEnumValue : public EnumValue
{
  private:
    T value;
    
  public:
    ConcreteEnumValue(const std::string& name, T value) : EnumValue(name), value(value) { }
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
    EnumSetting(const std::string& name, std::string ident, EnumSet values, EnumValue* value) : ConcreteSetting<EnumValue*>(SETTING_ENUM, name, ident, value), values(values)  { }
  
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
private:
  std::string name;
  ButtonStatus mask;
  ButtonStatus defaultMask;
  u8 shiftAmount;
  bool canBeRebound;
  bool allowsMultikey;

public:
  ButtonSetting(const std::string& name, GCWKey key, u8 shiftAmount, bool canBeRebound, bool allowsMultikey = false) :
    name(name),
    mask(Keys::shiftMaskForKey(key)),
    defaultMask(Keys::shiftMaskForKey(key)),
    shiftAmount(shiftAmount),
    canBeRebound(canBeRebound),
    allowsMultikey(allowsMultikey)
  { }
  
  ButtonSetting(const std::string& name, ButtonStatus mask, ButtonStatus defaultMask, u8 shiftAmount, bool canBeRebound, bool allowsMultikey) :
  name(name),
  mask(mask),
  defaultMask(defaultMask),
  shiftAmount(shiftAmount),
  canBeRebound(canBeRebound),
  allowsMultikey(allowsMultikey)
  { }
  
  const std::string& getName() const { return name; }
  std::string mnemonic() const { return Keys::mnemonicForKeybind(mask); }
  ButtonStatus getMask() const { return mask; }
  ButtonStatus getDefaultMask() const { return defaultMask; }
  
  u8 getShiftAmount() const { return shiftAmount; }
  bool isRebindable() const { return canBeRebound; }
  bool canBeMultikey() const { return allowsMultikey; }
  
  void setMask(GCWKey key) { mask = Keys::shiftMaskForKey(key); }
  void setMask(std::initializer_list<GCWKey> keys)
  {
    mask = 0;
    for (auto key : keys)
      mask |= Keys::shiftMaskForKey(key);
  }
  void setMask(ButtonStatus mask) { this->mask = mask; }
  
  void restore() { mask = defaultMask; }
  
  bool isSingleKey() const { return numberOfKeys() == 1; }
  u32 numberOfKeys() const
  {
    u32 i = mask;
    i = i - ((i >> 1) & 0x55555555);
    i = (i & 0x33333333) + ((i >> 2) & 0x33333333);
    return (((i + (i >> 4)) & 0x0F0F0F0F) * 0x01010101) >> 24;
  }
};

}

#endif
