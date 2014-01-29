#ifndef _CORE_INTERFACE_H_
#define _CORE_INTERFACE_H_

#include <vector>
#include <string>
#include <stdint.h>
#include <SDL.h>

#include "defines.h"
#include "settings.h"



namespace gcw {

class CoreInterface;


  

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
    std::vector<ButtonSetting> buttons;
  
	protected:
		void registerExtension(std::string ext) { extensions.push_back(ext); }
    void registerInformations(ConsoleType type, std::string ident, std::string name, std::string version) { information = CoreInfo(type,ident,name,version); }
    void registerSetting(Setting *setting) { settings.push_back(setting); }
    void registerButton(ButtonSetting button) { buttons.push_back(button); }

	public:
		virtual ~CoreInterface() { } // TODO: possible leaks of objects if _fini is not supported by the platform, fix it with a specific
		
    virtual void run(int argc, char **argv) = 0;
  
    virtual void setButtonStatus(ButtonStatus status) = 0;

    CoreInfo info() { return information; }
		std::vector<std::string> supportedExtensions() { return extensions; }
    std::vector<Setting*> supportedSettings() { return settings; }
    std::vector<ButtonSetting> supportedButtons() { return buttons; }
};
  
}

#endif
