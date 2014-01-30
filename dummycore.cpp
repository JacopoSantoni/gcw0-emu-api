#include "emu_interface.h"

using namespace std;
using namespace gcw;

enum KeyShiftAmount : u8
{
  KEY_A_SHIFT = 0,
  KEY_B_SHIFT = 1,
  KEY_X_SHIFT = 2,
  KEY_Y_SHIFT = 3,
  
  KEY_Z_SHIFT = 4
};

class DummyCore : public CoreInterface
{
  private:

  public:
    DummyCore()
    {
      registerInformations(CONSOLE_SUPER_NINTENDO, "dummy", "dummy", "1.0");
      registerExtension("smc");
      
      registerSetting(new BoolSetting("Auto Save SRAM", "auto-save-sram", false));
      registerSetting(new BoolSetting("Sound", "enable-sound", false));
      
      EnumSet<int32_t> sampleRates = {EnumValue<int32_t>("0",0),EnumValue<int32_t>("11025",11025),EnumValue<int32_t>("22050",22050),EnumValue<int32_t>("44100",44100),EnumValue<int32_t>("48000",48000)};
      registerSetting(new EnumSetting<int32_t>("Sample Rate", "sample-rate", sampleRates, sampleRates[3]));
      
      registerSetting(new BoolSetting("Stereo", "enable-sound-stereo", false));
      
      EnumSet<int32_t> frameSkip = {EnumValue<int32_t>("AUTO",-1),EnumValue<int32_t>("0",11025),EnumValue<int32_t>("1",1),EnumValue<int32_t>("2",2),EnumValue<int32_t>("3",3),EnumValue<int32_t>("4",4),EnumValue<int32_t>("5",5)};
      registerSetting(new EnumSetting<int32_t>("Frameskip", "frameskip", frameSkip, frameSkip[0]));
      
      registerSetting(new BoolSetting("Transparency", "transparency", false));
      registerSetting(new BoolSetting("Show FPS", "show-fps", false));
            
      registerButton(ButtonSetting("A", GCW_KEY_A, KEY_A_SHIFT, true));
      registerButton(ButtonSetting("B", GCW_KEY_B, KEY_B_SHIFT, true));
      registerButton(ButtonSetting("X", GCW_KEY_X, KEY_X_SHIFT, true));
      registerButton(ButtonSetting("Y", GCW_KEY_Y, KEY_Y_SHIFT, true));
      
      // this enables a joypad direction as a digital button
      registerButton(ButtonSetting("Z", GCW_ANALOG_LEFT, KEY_Z_SHIFT, true));
      
      // this instead enables the joypad for normal analog use, of course they can't be both active!
      enableNormalAnalogJoypad();
      
      
      setAnalogDeadZone(0.02f, 1.0f);

    }

    virtual void run(int argc, char **argv) { /*mainEntry(argc, argv);*/ }
  
    virtual void setButtonStatus(ButtonStatus status) { /* whatever */ }
    virtual void setAnalogStatus(AnalogStatus status) { /* whatever */ }
};

static DummyCore emulator;

extern "C" CoreInterface* retrieve()
{
	return &emulator;
}

DummyCore core;