#ifndef _CORE_VIEW_H_
#define _CORE_VIEW_H_

#include <SDL.h>
#include "../core/emu_interface.h"

#include "../gfx/view.h"
#include "../gfx/gfx.h"
#include "../gfx/blitter.h"
#include "../gfx/ui.h"
#include "../sfx/sfx.h"

namespace gcw {

struct ButtonDefinition
{
  bool enabled;
  u32 mask;
};

class Manager;
  
class CoreView : public View
{
private:
  const BlitterFactory* blitterFactory;
  Blitter *blitter;
  GfxBuffer buffer;
  Offset offset;

  std::unique_ptr<AudioOut> audioOut;
  std::unique_ptr<u32[]> audioBuffer;

  struct IntegralDeadZone { s32 min, max, delta; };
  
  
  ButtonDefinition mapping[GCW_KEY_COUNT+GCW_ANALOG_COUNT];
  IntegralDeadZone analogDeadZone;
  AnalogMode analogMode;
  
  
  ButtonStatus status;
  AnalogStatus analogStatus;
  
  ButtonStatus suspendShortcut;

  CoreInterface *core;
  
  void updatedBlitter();


public:
  CoreView(Manager *manager) : View(manager), blitterFactory(nullptr), blitter(nullptr) { }

  void initForCore(CoreInterface* core, const CorePreferences& preferences);
  void initControls();
  void initGfx(const std::string& scaler);
  void initSfx();
  void reset();
  
  void activated() override { UI::disableKeyRepeat(); }
  void deactivated() override { UI::enableKeyRepeat(); }
  
  void render() override;
  void handleEvents() override;
  
  void setBlitter(const std::string& name);

  void flushAudioBuffer()
  {
    if (audioOut)
      audioOut->clear();
  }
  
  const BlitterFactory* computeNativeBlitter(const GfxBufferSpec& spec);

  const AudioStatus& writeAudioSamples(size_t count, size_t shift)
  {
    const AudioStatus& astatus = audioOut->write(reinterpret_cast<u32*>(audioBuffer.get()), count);
    std::memmove(audioBuffer.get(), reinterpret_cast<u32*>(audioBuffer.get()) + count, shift + sizeof(*audioBuffer.get()));
    return std::move(astatus);
  }
  
  template<typename T>
  void setSettingOnCore(ConcreteSetting<T>* setting) { }
  
  template<typename T>
  void setSettingOnCore(ConcreteSetting<EnumValue<T>>* setting)
  {
    SettingValue<T> value(setting->getIdent(), setting->getType(), setting->getValue());
    core->settingChanged(value);
  }


  std::unique_ptr<AudioOut>& getAudioOut() { return audioOut; }
};

template<>
inline void CoreView::setSettingOnCore(ConcreteSetting<bool>* setting)
{
  SettingValue<bool> value(setting->getIdent(), setting->getType(), setting->getValue());
  core->settingChanged(value);
}
  
template<>
inline void CoreView::setSettingOnCore(ConcreteSetting<std::string>* setting)
{
  SettingValue<std::string> value(setting->getIdent(), setting->getType(), setting->getValue());
  core->settingChanged(value);
}
  
}


#endif
