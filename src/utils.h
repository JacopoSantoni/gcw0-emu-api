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
    
      void trimEndSlash() { if (path.back() == '/') path.pop_back(); }
      void trimStartSlash() { if (path.front() == '/') path.erase(1); }
      
    public:
      Path(const char *path);
      Path(std::string path);
      const std::string &value() const { return path; }
      void set(std::string &value) { path = value; }
      void append(std::string component);
      void removeLast();
      bool isRoot() const;
      
      
      std::string fileInsidePath(const std::string file) const;
      std::vector<std::string> findFiles(std::string ext, bool recursive) const;
      std::vector<std::string> findFiles(std::unordered_set<std::string> exts, bool recursive) const;
      std::vector<std::string> subfolders() const;
  };
  
  inline Path operator+(Path lhs, const Path& rhs) // first arg by value, second by const ref
  {
    lhs.append(rhs.value());
    return lhs;
  }
  
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
  
  class Text
  {
    public:
      static std::string clipText(const std::string &text, s32 length, const char *filler = nullptr);
      static const char* nameForKey(GCWKey key);
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
