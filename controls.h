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
  
class Controls
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
    void handleEvents();
};
  
}


#endif
