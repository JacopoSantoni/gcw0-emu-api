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
  
class Setting
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

private:
  Type type;
  Group group;
  std::string name;
  std::string ident;
  bool modifiableAtRuntime;

public:
  Setting(Type type, Group group, const std::string& name, const std::string& ident, bool modifiableAtRuntime) :
type(type), group(group), name(name), ident(ident), modifiableAtRuntime(modifiableAtRuntime) { }
  const std::string& getName() const { return name; }
  const std::string& getIdent() const { return ident; }
  Type getType() const { return type; }
  Group getGroup() const { return group; }
  bool canBeModifiedAtRuntime() const { return modifiableAtRuntime; }

  virtual ~Setting() { }
};
  
template<typename V>
class ConcreteSetting : public Setting
{
protected:
  V value;
  std::function<void(V)> callback;

public:
  ConcreteSetting(Type type, Group group, const std::string& name, const std::string& ident, V value, bool modifiableAtRuntime, std::function<void(V)> callback = [](const V value){ }) :
  Setting(type, group, name, ident, modifiableAtRuntime), value(value), callback(callback) { }
  const V& getValue() const { return value; }
  
  void setValue(V value) {
    this->value = value;
    callback(this->value);
  }
};

class BoolSetting : public ConcreteSetting<bool>
{
public:
  BoolSetting(Group group, const std::string& name, const std::string ident, bool value, bool modifiableAtRuntime, std::function<void(bool)> callback = [](const bool value){ }) :
  ConcreteSetting(Type::BOOLEAN, group, name, ident, value, modifiableAtRuntime, callback) { }
};
  
class PathSetting : public ConcreteSetting<std::string>
{
public:
  PathSetting(Group group, const std::string& name, const std::string ident, const std::string value, bool modifiableAtRuntime, std::function<void(std::string)> callback = [](const std::string value){ }) :
  ConcreteSetting(Type::PATH, group, name, ident, value, modifiableAtRuntime, callback) { }
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

  class EnumSetting
  {
    public:
      virtual void next() = 0;
      virtual void prev() = 0;
      virtual const std::string& getName() const = 0;
      virtual const std::string& getValueName() const = 0;
      virtual std::vector<std::string> getValueNames() const = 0;
      virtual bool canBeModifiedAtRuntime() const = 0;
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

  ConcreteEnumSetting<T>(Setting::Group group, const std::string& name, std::string ident, EnumSet<T>& values, size_t defaultValue, bool modifiableAtRuntime, std::function<void(EnumValueRef<T>)> callback = [](const EnumValueRef<T> value){ }) :
  ConcreteSetting<EnumValueRef<T>>(Setting::Type::ENUMERATION, group, name, ident, std::cref(values[defaultValue]), modifiableAtRuntime, callback),
  values(values), defaultValue(std::cref(this->values[defaultValue]))
  {
    this->value = this->defaultValue;
  }

  const std::string& getValueName() const override { return getValue().get().getName(); }
  const std::string& getName() const override { return ConcreteSetting<EnumValueRef<T>>::getName(); }
  bool canBeModifiedAtRuntime() const override { return ConcreteSetting<EnumValueRef<T>>::canBeModifiedAtRuntime(); }

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

  std::vector<std::string> getValueNames() const override
  {
    std::vector<std::string> names;
    std::transform(values.begin(), values.end(), std::back_inserter(names), [](const EnumValue<T>& value){ return value.getName(); });
    return names;
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
