#ifndef _CORE_INTERFACE_H_
#define _CORE_INTERFACE_H_

#include <vector>
#include <string>
#include <memory>
#include <stdint.h>
//#include <SDL.h>

#include "manager_interface.h"
#include "../common/defines.h"
#include "../common/optional.h"
#include "../data/settings.h"


namespace gcw {

class CoreInterface;

  struct CoreDetails
  {
    std::string name;
    std::string author;
    std::string porter;
    std::string webpage;
    
    CoreDetails() = default;
    CoreDetails(const std::string& name, const std::string& author, const std::string& porter, const std::string& webpage) : name(name), author(author), porter(porter), webpage(webpage) { }
    CoreDetails(const std::string& name) : name(name) { }
  };
  
  enum class CoreFeature : u32
  {
    REQUIRES_MULTI_THREADING_LOADING = 0x00000001,
    
    CAN_SAVE_STATE                   = 0x00010000,
    CAN_SAVE_RAM                     = 0x00020000,
    CAN_SOFT_RESET                   = 0x00040000,
  };
  
  typedef u32 CoreFeatures;
  
struct CoreInfo
{
private:
  std::vector<ButtonSetting> buttons;

public:
  std::vector<System::Type> type;

  CoreIdentifier ident;
  CoreDetails details;
  time_t timestamp;
  
  CoreInfo() : type({System::Type::UNCATEGORISED}), ident(CoreIdentifier()) { }
  
  void setButtons(const std::vector<ButtonSetting>& buttons) { this->buttons = buttons; }
  
  void setInformations(std::initializer_list<System::Type> type, const std::string& ident, const std::string& name, const std::string& version)
  {
    this->type = type;
    this->ident = CoreIdentifier(ident, version);
    this->details = CoreDetails(name);
  }
  
  const std::vector<ButtonSetting>& supportedButtons() const { return buttons; }
  void registerButton(const ButtonSetting& button) { buttons.push_back(button); }
  
  const std::string identifier() const { return ident.identifier(); }
  const std::string title() const { return details.name + " (" + ident.version + ")"; }
  
  bool operator==(const CoreInfo& other) const { return ident == other.ident; }
  bool operator!=(const CoreInfo& other) const { return ident != other.ident; }
  
  CoreInfo& operator=(const CoreInfo&) = delete;
  
  friend class Unserializer;
};
  
  
  
  class CoreInterface
  {
  private:
    CoreInfo information;
    
    CoreFeatures features;
  
    std::vector<std::unique_ptr<Setting>> settings;
    std::vector<ButtonSetting> buttons;
    bool analogJoypadEnabled;
    AnalogDeadZone analogDeadZone;
  
    GfxBufferSpec gfxFormat;
    std::optional<SfxAudioSpec> sfxFormat;
  

  protected:
    CoreInterface() : features(0), analogJoypadEnabled(false) { }
  
    void registerFeature(CoreFeature features) { this->features |= static_cast<CoreFeatures>(features); }
    void registerInformations(std::initializer_list<System::Type> systems, std::string ident, std::string name, std::string version) { information.setInformations(systems,ident,name,version); }
    void registerInformations(System::Type type, std::string ident, std::string name, std::string version) { registerInformations({type},ident,name,version); }
    void registerSetting(Setting *setting) { settings.push_back(std::unique_ptr<Setting>(setting)); }
    void registerButton(const ButtonSetting& button) { information.registerButton(button); }
    void setAnalogDeadZone(float min, float max ) { analogDeadZone.min = min; analogDeadZone.max = max; }
    void enableNormalAnalogJoypad() {  analogJoypadEnabled = true; }
    
    void setGfxFormat(u16 width, u16 height, GfxBufferFormat format) { gfxFormat = {width, height, format}; }
    void setSfxFormat(SfxAudioSpec format) { sfxFormat = std::optional<SfxAudioSpec>(format); }
  
    u32* audioBuffer;
    GfxBuffer gfxBuffer;
    ButtonStatus buttonStatus;
    AnalogStatus analogStatus;
    
    ManagerInterface* manager;
    
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
    
    virtual void initialize() = 0;
    virtual void releaseResources() = 0;
    
    virtual void emulationFrame() = 0;
    virtual void loadRomByFileName(const std::string& name) = 0;
  
    virtual void emulationStarted() = 0;
    virtual void emulationSuspended() = 0;
    virtual void emulationResumed() = 0;
    virtual void emulationStopped() = 0;
    
    virtual void softReset() { }
    virtual void reset() = 0;
    
    virtual void stateSaveTo(const std::string& filename) { }
    virtual void stateLoadFrom(const std::string& filename) { }
    
    virtual void sramSetPath(const std::string& path) { }
    virtual void sramSaveTo(const std::string& path, const std::string& romName) { }
    virtual void sramLoadFrom(const std::string& path, const std::string& romName) { }
  
  
    void setBuffer(GfxBuffer buffer) { this->gfxBuffer = buffer; }
    void setAudioBuffer(u32* buffer) { this->audioBuffer = buffer; }

    const AudioStatus& writeAudioSamples(size_t count, size_t shift) { return manager->writeAudioSamples(count, shift); }
  
  
  
  
    bool hasFeature(CoreFeature feature) const { return (features & static_cast<CoreFeatures>(feature)) != 0; }

    const std::vector<std::unique_ptr<Setting>>& supportedSettings() const { return settings; }
  
    AnalogDeadZone getAnalogDeadZone() const { return analogDeadZone; }
    bool isAnalogJoypadUsed() const { return analogJoypadEnabled; }
  
    const GfxBufferSpec& getGfxSpec() const { return gfxFormat; }
    const std::optional<SfxAudioSpec>& getSfxSpec() const { return sfxFormat; }
    
    const CoreInfo& info() const { return information; }
  };
  
}

#endif
