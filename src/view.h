#ifndef _VIEW_H_
#define _VIEW_H_

namespace gcw
{
  
  class Manager;
  class Gfx;

  class View
  {
    protected:
      Manager* const manager;
      Gfx* const gfx;
    
    public:
      View(Manager *manager);
      virtual void render() = 0;
      virtual void handleEvents() = 0;
  };
  
}


#endif