#ifndef __LOADING_VIEW_H__
#define __LOADING_VIEW_H__

#include "../gfx/view.h"

namespace gcw
{

  class LoadingView : public View
  {
  private:
    float percent;
    
  public:
    LoadingView(Manager* manager);
    
    void resetForFile(const std::string& name);
    
    void render() override;
    void handleEvents() override;
    
  };
    
}


#endif