#ifndef _CORE_VIEW_H_
#define _CORE_VIEW_H_

#include <SDL.h>
#include "../core/emu_interface.h"
#include "../ui/view.h"

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
    GfxBuffer buffer;
    Offset offset;
  
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
    CoreView(Manager *manager) : View(manager) { }
  
    void initForCore(CoreInterface* core, ButtonStatus suspendKeys);
    void initControls(ButtonStatus suspendKeys);
    void initGfx();
    void reset();
  
    virtual void render();
    virtual void handleEvents();
};
  
}


#endif
