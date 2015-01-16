#ifndef __HELP_H__
#define __HELP_H__

#include "../common/defines.h"

#include <string>
#include <vector>

namespace gcw {

class Manager;
  
enum class HelpType : u16
{
  SUBMENU,
  BOOL_SETTING,
  ENUM_SETTING,
  PATH_SETTING,
  
  SETTING_CANT_BE_CHANGED,
  
  LAUNCH_ROM,
  
  NONE
};
  
class Help
{
public:

  
private:
  Manager* manager;
  std::string message;
  
  struct Entry
  {
    std::vector<GCWKey> key;
    std::string text;
  };
  
  void buildMessage(const std::vector<Entry>& entries);
  
public:
  Help(Manager* manager);
  
  void set(HelpType type);
  void clear();
  
  void render();
};
  
}



#endif