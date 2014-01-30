#ifndef _CONTROLS_H_
#define _CONTROLS_H_

#include <SDL.h>
#include "emu_interface.h"

namespace gcw {

struct ButtonDefinition
{
  bool enabled;
  u32 mask;
};
  
class ControlsHandler
{
  public:
    virtual void handleEvents() = 0;
};
  
class CoreControlsHandler : public ControlsHandler
{
  private:
    struct IntegralDeadZone { s16 min, max, delta; };
    
    s8 indexForKey(GCWKey key);
    ButtonDefinition mapping[GCW_KEY_COUNT];
    ButtonDefinition analogMapping[GCW_ANALOG_COUNT];
    IntegralDeadZone analogDeadZone;
    AnalogMode analogMode;
    
    ButtonStatus status;
    AnalogStatus analogStatus;

  public:
    void initControls(CoreInterface *core);
  
    virtual void handleEvents();
};
  
class Controls
{
  private:
    CoreControlsHandler coreHandler;
    ControlsHandler *handler;
  
  public:
    Controls() : handler(&coreHandler) { }
    void handleEvents() { handler->handleEvents(); }
  
    ControlsHandler *current() { return handler; }
};
  
}


#endif
