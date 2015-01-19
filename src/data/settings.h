#ifndef _SETTINGS_H_
#define _SETTINGS_H_

#include "../common/defines.h"
#include "../common/keys.h"

#include <string>
#include <vector>
#include <algorithm>
#include <functional>


namespace gcw
{

template<typename T>
class EnumValue
{
  private:
    const std::string name;
    T value;
  
  public:
    EnumValue(const std::string& name, T value) : name(name), value(value) { }
    const std::string& getName() const { return name; }
  
    const T& getValue() const { return value; }
  
    bool operator==(const EnumValue& rhs) const { return name == rhs.name; }
  
    virtual ~EnumValue() { }
};
  
template<typename T> using EnumSet = std::vector<EnumValue<T>>;

struct Setting
{
public:
  enum class Type : u8
  {
    BOOLEAN,
    PATH,
    ENUMERATION,
  };
  
  enum class Group : u8
  {
    VIDEO,
    AUDIO,
    MISC
  };
  
  Setting::Type type;
  Setting::Group group;
  std::string ident;
  std::string name;
  std::vector<std::string> values;
  std::string defaultValue;
  bool canBeModifiedAtRuntime;
  
  Setting(const std::string& ident, const std::string& name, Setting::Group group, bool defaultValue, bool canBeModifiedAtRuntime) :
  type(Setting::Type::BOOLEAN), group(group), ident(ident), name(name), values{{}}, defaultValue(defaultValue ? "true" : "false"), canBeModifiedAtRuntime(canBeModifiedAtRuntime) { }
  
  Setting(const std::string& ident, const std::string& name, Setting::Group group, const std::string& defaultValue, bool canBeModifiedAtRuntime) :
  type(Setting::Type::PATH), group(group), ident(ident), name(name), values{{}}, defaultValue(defaultValue), canBeModifiedAtRuntime(canBeModifiedAtRuntime) { }
  
  Setting(const std::string& ident, const std::string& name, Setting::Group group, const std::vector<std::string>& values, const std::string& defaultValue, bool canBeModifiedAtRuntime) :
  type(Setting::Type::ENUMERATION), group(group), ident(ident), name(name), values(values), defaultValue(defaultValue), canBeModifiedAtRuntime(canBeModifiedAtRuntime) { }
};

class SettingHandle
{
private:
  std::string ident;
  
public:
  SettingHandle(const std::string& ident) : ident(ident) { }
  
  const std::string& getIdent() { return ident; }
  virtual void valueChanged(const std::string& value) = 0;
};

class BoolSettingHandle : public SettingHandle
{
private:
  std::function<void(bool)> callback;
  
public:
  BoolSettingHandle(const std::string& ident, const std::function<void(bool)> callback) : SettingHandle(ident), callback(callback) { }
  void valueChanged(const std::string& value) override { callback(value == "true" ? true : false); }
};

class PathSettingHandle : public SettingHandle
{
private:
  std::function<void(std::string)> callback;
  
public:
  PathSettingHandle(const std::string& ident, const std::function<void(std::string)> callback) : SettingHandle(ident), callback(callback) { }
  void valueChanged(const std::string& value) override { callback(value); }
};

template<typename T>
class EnumSettingHandle : public SettingHandle
{
private:
  const EnumSet<T>& values;
  std::function<void(T)> callback;
  
public:
  EnumSettingHandle(const std::string& ident, const EnumSet<T>& values, const std::function<void(T)> callback) : SettingHandle(ident), values(values), callback(callback) { }
  void valueChanged(const std::string& value) override
  {
    const auto it = std::find_if(values.begin(), values.end(), [&value](const EnumValue<T>& evalue) { return evalue.getName() == value; });
    if (it != values.end())
      callback(it->getValue());
  }
};


struct ButtonSetting
{
private:
  std::string name;
  mutable ButtonStatus mask;
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
  
  void setMask(GCWKey key) const { mask = Keys::shiftMaskForKey(key); }
  void setMask(std::initializer_list<GCWKey> keys) const
  {
    mask = 0;
    for (auto key : keys)
      mask |= Keys::shiftMaskForKey(key);
  }
  void setMask(ButtonStatus mask) const { this->mask = mask; }
  
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
