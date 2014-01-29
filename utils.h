#ifndef _UTILS_H_
#define _UTILS_H_

#include <dirent.h>

#include <vector>
#include <string>

namespace gcw
{
  class Files
  {
    public:
      static std::vector<std::string> findFiles(std::string path, std::string ext);
      static std::vector<std::string> findFiles(std::string path, std::vector<std::string> exts);

    
  };
}

#endif
