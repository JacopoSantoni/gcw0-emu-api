#ifndef _UTILS_H_
#define _UTILS_H_

#include "defines.h"

#include <dirent.h>

#include <chrono>
#include <unordered_set>
#include <vector>
#include <string>


namespace gcw
{  
  class Files
  {
    public:
      static std::vector<std::string> findFiles(std::string path, std::string ext, bool recursive);
      static std::vector<std::string> findFiles(std::string path, std::unordered_set<std::string> exts, bool recursive);
      static std::vector<std::string> findSubfolders(std::string path);

    
  };
  
  class Path
  {
    private:
      std::string path;
      
    public:
      Path(const char *path);
      Path(std::string path);
      std::string &value() { return path; }
      void append(std::string component);
      void removeLast();
      bool isRoot();
      
      
      std::string fileInsidePath(const std::string file);
      std::vector<std::string> findFiles(std::string ext, bool recursive);
      std::vector<std::string> findFiles(std::unordered_set<std::string> exts, bool recursive);
      std::vector<std::string> subfolders();
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
    
    public:
      Uncopyable() { }
  };
}

#endif
