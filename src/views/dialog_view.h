#ifndef __DIALOG_VIEW_H__
#define __DIALOG_VIEW_H__

#include "../gfx/view.h"

#include <functional>

namespace gcw
{
  class Manager;
  
  class DialogView : public View
  {
  private:
    
  public:
    DialogView(Manager* manager);
    
    void handleEvents() override;
    void render() override;
  };
  
}



#endif