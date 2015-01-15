#ifndef _SETTINGS_H_
#define _SETTINGS_H_

#include "../common/defines.h"
#include "../common/keys.h"

#include <string>
#include <vector>
#include <algorithm>


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
    const std::string& getName() const { return name; }
    const std::string& getIdent() const { return ident; }
    SettingType getType() const { return type; }
  
    virtual ~Setting() { }
};

template<typename V>
class ConcreteSetting : public Setting
{
  private:
    V value;
  
  public:
    ConcreteSetting(SettingType type, const std::string& name, const std::string& ident, V value) : Setting(type, name, ident), value(value) { }
    const V& getValue() const { return value; }
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

  
template<typename T>
class EnumValue
{
  private:
    const std::string name;
    T value;
  
  public:
    EnumValue(const std::string& name, T value) : name(name), value(value) { }
    const std::string& getName() const { return name; }
  
    bool operator==(const EnumValue& rhs) const { return name == rhs.name; }
  
    virtual ~EnumValue() { }
};
  
template<typename T> using EnumSet = std::vector<EnumValue<T>>;
template<typename T> using EnumValueRef = std::reference_wrapper<const EnumValue<T>>;

//typedef std::vector<std::unique_ptr<EnumValue>> EnumSet;
  
/*class EnumSetting
{
  private:
  
  public:
    virtual size_t count() = 0;

};*/
  
  class EnumSetting
  {
    public:
      virtual void next() = 0;
      virtual void prev() = 0;
      virtual const std::string& getName() const = 0;
      virtual const std::string& getValueName() const = 0;
  };

template<typename T>
class ConcreteEnumSetting : public ConcreteSetting<EnumValueRef<T>>, public EnumSetting
{
  private:
    EnumSet<T> values;
    EnumValueRef<T> defaultValue;
  
  public:
    using ConcreteSetting<EnumValueRef<T>>::getValue;
    using ConcreteSetting<EnumValueRef<T>>::setValue;
  
    ConcreteEnumSetting<T>(const std::string& name, std::string ident, EnumSet<T>& values, size_t defaultValue) : ConcreteSetting<EnumValueRef<T>>(SETTING_ENUM, name, ident, std::cref(values[defaultValue])),
    values(values), defaultValue(std::cref(this->values[defaultValue]))
    {
      setValue(this->defaultValue);
    }
  
    const std::string& getValueName() const { return getValue().get().getName(); }
    const std::string& getName() const { return ConcreteSetting<EnumValueRef<T>>::getName(); }
  
    void next() override {
      const EnumValueRef<T>& current = getValue();
      if (current.get() == values.back())
        setValue(values.front());
      else
      {
        auto it = find(values.begin(), values.end(), current);
        setValue(*(++it));
      }
    }
  
    void prev() override {
      const EnumValue<T>& current = getValue();
      if (current == values.front())
        setValue(values.back());
      else
      {
        auto it = find(values.begin(), values.end(), current);
        setValue(*(--it));
      }
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
