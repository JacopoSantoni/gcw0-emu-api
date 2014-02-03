#ifndef _VIEW_H_
#define _VIEW_H_

namespace gcw
{
  
  class Manager;

class View
{
  protected:
    Manager* const manager;
    
  public:
    View(Manager *manager) : manager(manager) { }
    virtual void render() = 0;
    virtual void handleEvents() = 0;
};
  
}


#endif