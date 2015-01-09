#ifndef _CORE_VIEW_H_
#define _CORE_VIEW_H_

#include <SDL.h>
#include "../core/emu_interface.h"
#include "../gfx/view.h"
#include "../gfx/gfx.h"
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
    Blitter *blitter;
    GfxBuffer buffer;
    Offset offset;
  
    std::unique_ptr<AudioOut> audioOut;
    std::unique_ptr<u32[]> audioBuffer;
  
  
    struct IntegralDeadZone { s32 min, max, delta; };
        
    s8 indexForKey(GCWKey key);
    ButtonDefinition mapping[GCW_KEY_COUNT+GCW_ANALOG_COUNT];
    IntegralDeadZone analogDeadZone;
    AnalogMode analogMode;
    
    
    ButtonStatus status;
    AnalogStatus analogStatus;
    
    ButtonStatus suspendShortcut;
  
    CoreInterface *core;
  
  public:
    CoreView(Manager *manager) : View(manager), blitter(nullptr) { }
  
    void initForCore(CoreInterface* core, ButtonStatus suspendKeys);
    void initControls(ButtonStatus suspendKeys);
    void initGfx();
    void reset();
  
    virtual void render();
    virtual void handleEvents();
  
    const AudioStatus& writeAudioSamples(size_t count, size_t shift)
    {
      const AudioStatus& astatus = audioOut->write(reinterpret_cast<u32*>(audioBuffer.get()), count);
      std::memmove(audioBuffer.get(), reinterpret_cast<u32*>(audioBuffer.get()) + count, shift + sizeof(*audioBuffer.get()));
      return std::move(astatus);
    }
};
  
}


#endif
