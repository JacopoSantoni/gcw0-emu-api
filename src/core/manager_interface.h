#ifndef __MANAGER_INTERFACE_H__
#define __MANAGER_INTERFACE_H__

namespace gcw
{
  class ManagerInterface
  {
  public:
    virtual void reportRomLoading(float percent) = 0;
    virtual void reportRomLoaded() = 0;
    
  };
}

#endif