#ifndef _CORE_INTERFACE_H_
#define _CORE_INTERFACE_H_

#include <vector>
#include <string>
#include <stdint.h>

#include "settings.h"

typedef uint8_t u8;
typedef uint16_t u16;

class CoreInterface;

enum ConsoleType : u16
{
  CONSOLE_UNSPECIFIED,
  
  CONSOLE_GAME_BOY,
  CONSOLE_SUPER_NINTENDO
};

struct CoreInfo
{
  ConsoleType type;
  std::string ident;
  std::string name;
  std::string version;
  
  CoreInfo(ConsoleType type, std::string ident, std::string name, std::string version) :  type(type), ident(ident), name(name), version(version) { }
  CoreInfo() : type(CONSOLE_UNSPECIFIED), ident(std::string()), name(std::string()), version(std::string()) { }
};




class CoreInterface
{
	private:
		std::vector<std::string> extensions;
    CoreInfo information;
  
    std::vector<Setting*> settings;
  
	protected:
		void registerExtension(std::string ext) { extensions.push_back(ext); }
    void registerInformations(ConsoleType type, std::string ident, std::string name, std::string version) { information = CoreInfo(type,ident,name,version); }
    void registerSetting(Setting *setting) { settings.push_back(setting); }

	public:
		virtual ~CoreInterface() { }
		virtual void run(int argc, char **argv) = 0;

    CoreInfo info() { return information; }
		std::vector<std::string> supportedExtensions() { return extensions; }
    std::vector<Setting*> supportedSettings() { return settings; }
};

#endif
