#ifndef __MANAGER_INTERFACE_H__
#define __MANAGER_INTERFACE_H__

namespace gcw
{
  struct AudioStatus;
  
  class ManagerInterface
  {
  public:
    virtual void reportRomLoading(float percent) = 0;
    virtual void reportRomLoaded() = 0;
    virtual const AudioStatus& writeAudioSamples(size_t count, size_t shift) = 0;
  };
}

#endif