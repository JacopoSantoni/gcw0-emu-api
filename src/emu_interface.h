#ifndef _CORE_INTERFACE_H_
#define _CORE_INTERFACE_H_

#include <vector>
#include <string>
#include <memory>
#include <stdint.h>
#include <SDL.h>

#include "defines.h"
#include "gfx.h"
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
  
    std::vector<std::unique_ptr<Setting> > settings;
    std::vector<ButtonSetting> buttons;
    bool analogJoypadEnabled;
    AnalogDeadZone analogDeadZone;
  
    GfxBufferSpec gfxFormat;
  
  
	protected:
    CoreInterface() : analogJoypadEnabled(false) { }
  
		void registerExtension(std::string ext) { extensions.push_back(ext); }
    void registerInformations(ConsoleType type, std::string ident, std::string name, std::string version) { information = CoreInfo(type,ident,name,version); }
    void registerSetting(Setting *setting) { settings.push_back(std::unique_ptr<Setting>(setting)); }
    void registerButton(ButtonSetting button) { buttons.push_back(button); }
    void setAnalogDeadZone(float min, float max ) { analogDeadZone.min = min; analogDeadZone.max = max; }
    void enableNormalAnalogJoypad() {  analogJoypadEnabled = true; }
  
    void setGfxFormat(u16 width, u16 height, GfxBufferFormat format) { gfxFormat = {width, height, format}; }
  
    GfxBuffer gfxBuffer;
    ButtonStatus buttonStatus;
    AnalogStatus analogStatus;

	public:
		virtual ~CoreInterface() { } // TODO: possible leaks of objects if _fini is not supported by the platform, fix it with a specific
		
    virtual void run(int argc, char **argv) = 0;
  
    /**
     * Pass the current button status to the core. The status is built according to the button specification
     * of the core (@see registerButton)
     *
     * @param status a bitmask, a bit is set if the corresponding button is pressed, 0 otherwise.
     */
    virtual void setButtonStatus(ButtonStatus status) { buttonStatus = status; }
  
    virtual void setAnalogStatus(AnalogStatus status) { analogStatus = status; }
  
    virtual void emulationFrame() = 0;
    void setBuffer(GfxBuffer buffer) { this->gfxBuffer = buffer; }

    CoreInfo info() { return information; }
		std::vector<std::string> supportedExtensions() { return extensions; }
    std::vector<std::unique_ptr<Setting> >* supportedSettings() { return &settings; }
  
  
    std::vector<ButtonSetting> supportedButtons() { return buttons; }
    AnalogDeadZone getAnalogDeadZone() { return analogDeadZone; }
    bool isAnalogJoypadUsed() { return analogJoypadEnabled; }
  
    GfxBufferSpec getGfxSpec() { return gfxFormat; }
};
  
}

#endif
