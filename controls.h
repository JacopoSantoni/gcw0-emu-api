#ifndef _CONTROLS_H_
#define _CONTROLS_H_

#include <SDL.h>
#include "emu_interface.h"

namespace gcw {

struct ButtonDefinition
{
  bool enabled;
  u8 shiftAmount;
};
  
class Controls
{
  private:
    s8 indexForKey(GCWKey key);
    ButtonDefinition mapping[GCWKEY_AMOUNT];
    ButtonStatus status;
  
  
  public:
    void initControls(CoreInterface *core);
    void handleEvents();
};
  
}


#endif
