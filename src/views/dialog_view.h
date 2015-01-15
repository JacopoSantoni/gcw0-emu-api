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
    enum
    {
      MESSAGE,
      CONFIRM
    } type;
    
    typedef std::function<void(void)> void_lambda;
    
    std::string message;
    void_lambda yesLambda, noLambda;
    
  public:
    DialogView(Manager* manager);
    
    void handleEvents() override;
    void render() override;
    
    void initMessage(const std::string& message, void_lambda lambda)
    {
      this->message = message;
      this->yesLambda = lambda;
      this->type = MESSAGE;
    }
    
    void initConfirm(const std::string& message, void_lambda yesLambda, void_lambda noLambda)
    {
      this->message = message;
      this->yesLambda = yesLambda;
      this->noLambda = noLambda;
      this->type = CONFIRM;
    }
    
    
  };
  
}



#endif