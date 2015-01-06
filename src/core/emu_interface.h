#ifndef _CORE_INTERFACE_H_
#define _CORE_INTERFACE_H_

#include <vector>
#include <string>
#include <memory>
#include <stdint.h>
#include <SDL.h>

#include "manager_interface.h"
#include "../common/defines.h"
//#include "../ui/gfx.h"
#include "../data/settings.h"



namespace gcw {

class CoreInterface;

struct CoreInfo
{
  std::vector<System::Type> type;
  CoreIdentifier ident;
  std::string name;
  
  CoreInfo(std::initializer_list<System::Type> type, std::string ident, std::string name, std::string version) :  type(type), ident(CoreIdentifier(ident,version)), name(name) { }
  CoreInfo() : type({System::Type::UNCATEGORISED}), ident(CoreIdentifier()), name(std::string()) { }
  
  const std::string identifier() const { return ident.identifier(); }
  const std::string title() const { return name + " (" + ident.version + ")"; }
  
  bool operator==(const CoreInfo& other) const { return ident == other.ident; }
  bool operator!=(const CoreInfo& other) const { return ident != other.ident; }
};
  
  
  
  class CoreInterface
  {
  private:
    CoreInfo information;
  
    bool requiresProgressForLoading;
    std::vector<std::unique_ptr<Setting> > settings;
    std::vector<ButtonSetting> buttons;
    bool analogJoypadEnabled;
    AnalogDeadZone analogDeadZone;
  
    GfxBufferSpec gfxFormat;
  
    ManagerInterface* manager;
  
  
  protected:
    CoreInterface() : requiresProgressForLoading(false), analogJoypadEnabled(false) { }
  
    void registerInformations(std::initializer_list<System::Type> systems, std::string ident, std::string name, std::string version) { information = CoreInfo(systems,ident,name,version); }
    void registerInformations(System::Type type, std::string ident, std::string name, std::string version) { registerInformations({type},ident,name,version); }
    void registerSetting(Setting *setting) { settings.push_back(std::unique_ptr<Setting>(setting)); }
    void registerButton(ButtonSetting button) { buttons.push_back(button); }
    void setAnalogDeadZone(float min, float max ) { analogDeadZone.min = min; analogDeadZone.max = max; }
    void enableNormalAnalogJoypad() {  analogJoypadEnabled = true; }
    
    void enableProgressForLoading() { requiresProgressForLoading = true; }
  
    void setGfxFormat(u16 width, u16 height, GfxBufferFormat format) { gfxFormat = {width, height, format}; }
  
    GfxBuffer gfxBuffer;
    ButtonStatus buttonStatus;
    AnalogStatus analogStatus;

  public:
    virtual ~CoreInterface() { } // TODO: possible leaks of objects if _fini is not supported by the platform, fix it with a specific
    
    //virtual void run(int argc, char **argv) = 0;
  
    /**
     * Pass the current button status to the core. The status is built according to the button specification
     * of the core (@see registerButton)
     *
     * @param status a bitmask, a bit is set if the corresponding button is pressed, 0 otherwise.
     */
    virtual void setButtonStatus(ButtonStatus status) { buttonStatus = status; }
  
    /**
     * Pass the current analog pad status to the core. The status contains two normalized values according
     * to the dead zone specified through @see setAnalogDeadZone.
     *
     * @param status a struct containing two floats in range [0.0,1.0]
     */
    virtual void setAnalogStatus(AnalogStatus status) { analogStatus = status; }
  
    /**
     * Sets the manager to the core interface, so that it is possible for the core to communicate things to it.
     * This method is called by the manager when the core is effectively loaded to be used.
     *
     * @param manager the manager of the cores
     */
    void setManager(ManagerInterface* manager) { this->manager = manager; }
    
    virtual void emulationFrame() = 0;
    virtual void loadRomByFileName(const std::string& name) = 0;
  
    virtual void emulationStarted() = 0;
    virtual void emulationSuspended() = 0;
    
    virtual void initialize() = 0;
    virtual void releaseResources() = 0;
    virtual void reset() = 0;
  
  
    void setBuffer(GfxBuffer buffer) { this->gfxBuffer = buffer; }

  
  
  
  
    bool doesRequireProgressForLoading() const { return requiresProgressForLoading; }

    const CoreInfo& info() const { return information; }
    const std::vector<std::unique_ptr<Setting>>& supportedSettings() const { return settings; }
  
    const std::vector<ButtonSetting>& supportedButtons() const { return buttons; }
    AnalogDeadZone getAnalogDeadZone() const { return analogDeadZone; }
    bool isAnalogJoypadUsed() const { return analogJoypadEnabled; }
  
    const GfxBufferSpec& getGfxSpec() const { return gfxFormat; }
  };
  
}

#endif
