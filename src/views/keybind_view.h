#ifndef __KEYBIND_VIEW_H__
#define __KEYBIND_VIEW_H__

#include "../gfx/view.h"

#include <string>
#include <functional>
#include <chrono>

namespace gcw
{
  class Manager;
  
  class KeybindView : public View
  {
  public:
    typedef std::function<void(ButtonStatus)> mrebind_lambda;
    typedef std::function<void(GCWKey)> rebind_lambda;
    typedef std::function<void(void)> cancel_lambda;
    
  private:
    ButtonStatus mask;
    bool canBeMultiKey;
    rebind_lambda lambda;
    mrebind_lambda mlambda;
    cancel_lambda clambda;
    std::string caption;
    
    std::chrono::time_point<std::chrono::steady_clock> start;
    
  public:
    KeybindView(Manager* manager) : View(manager) { }
    
    void activated() override;
    void deactivated() override;
    
    void initm(mrebind_lambda lambda, cancel_lambda clambda, const std::string& caption);
    void init(rebind_lambda lambda, cancel_lambda clambda, const std::string& caption);
    
    void handleEvents() override;
    void render() override;
  };
}



#endif