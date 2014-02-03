#ifndef _UTILS_H_
#define _UTILS_H_

#include "defines.h"

#include <dirent.h>

#include <chrono>
#include <vector>
#include <string>

namespace gcw
{
  class Files
  {
    public:
      static std::vector<std::string> findFiles(std::string path, std::string ext, bool recursive);
      static std::vector<std::string> findFiles(std::string path, std::vector<std::string> exts, bool recursive);

    
  };
  
  class Timer
  {
    const float DEFAULT_FPS = 60.0f;
    
    std::chrono::steady_clock clock;
    std::chrono::microseconds ticksForFrame;
    
    u32 totalFrames;
    std::chrono::time_point<std::chrono::steady_clock> base;
    
    public:
      Timer() : ticksForFrame(static_cast<u32>(1000000 / DEFAULT_FPS)), totalFrames(0) { }
    
      void setFrameRate(float rate);
      void frameRateDelay();
  };
  
  class Uncopyable
  {
    private:
      Uncopyable( const Uncopyable& other ) = delete; // non construction-copyable
      Uncopyable& operator=( const Uncopyable& ) = delete; // non copyable
  };
}

#endif
