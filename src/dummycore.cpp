#include "emu_interface.h"

#include "gfx.h"

using namespace std;
using namespace gcw;

enum KeyShiftAmount : u8
{
  KEY_A_SHIFT = 0,
  KEY_B_SHIFT = 1,
  KEY_X_SHIFT = 2,
  KEY_Y_SHIFT = 3,
  
  KEY_LEFT_SHIFT = 4,
  KEY_RIGHT_SHIFT = 5,
  KEY_UP_SHIFT = 6,
  KEY_DOWN_SHIFT = 7
  
};
  
class DummyUtil
{
  public:
  static void rectFill(GfxBuffer buffer, int x, int y, int w, int h, u32 color)
  {
    u32* data = reinterpret_cast<u32*>(buffer.data);
    
    for (int i = x; i < x+w; ++i)
      for (int j = y; j < y+h; ++j)
        data[i + j*buffer.pitch] = color;
  }
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
      registerButton(ButtonSetting("Up", GCW_KEY_UP, KEY_UP_SHIFT, true));
      registerButton(ButtonSetting("Right", GCW_KEY_RIGHT, KEY_RIGHT_SHIFT, true));
      registerButton(ButtonSetting("Left", GCW_KEY_LEFT, KEY_LEFT_SHIFT, true));
      registerButton(ButtonSetting("Down", GCW_KEY_DOWN, KEY_DOWN_SHIFT, true));
      
      // this enables a joypad direction as a digital button
      //registerButton(ButtonSetting("Z", GCW_ANALOG_LEFT, KEY_Z_SHIFT, true));
      
      // this instead enables the joypad for normal analog use, of course they can't be both active!
      //enableNormalAnalogJoypad();
      
      
      setAnalogDeadZone(0.02f, 1.0f);
      
      setGfxFormat(240, 160, FORMAT_32BPP);

    }

    virtual void emulationFrame();
    virtual void run(int argc, char **argv) { /*mainEntry(argc, argv);*/ }
  
    virtual void setButtonStatus(ButtonStatus status) { /* whatever */ }
    virtual void setAnalogStatus(AnalogStatus status) { /* whatever */ }
};
  
void DummyCore::emulationFrame()
{
  Gfx::clear<u32>(gfxBuffer, Gfx::ccc(220,220,220));

  DummyUtil::rectFill(gfxBuffer, 210, 60, 20, 20, buttonStatus & (1<<KEY_A_SHIFT) ? Gfx::ccc(200, 0, 0) : Gfx::ccc(160, 160, 160));
  DummyUtil::rectFill(gfxBuffer, 185, 85, 20, 20, buttonStatus & (1<<KEY_B_SHIFT) ? Gfx::ccc(200, 0, 0) : Gfx::ccc(160, 160, 160));
  DummyUtil::rectFill(gfxBuffer, 160, 60, 20, 20, buttonStatus & (1<<KEY_X_SHIFT) ? Gfx::ccc(200, 0, 0) : Gfx::ccc(160, 160, 160));
  DummyUtil::rectFill(gfxBuffer, 185, 35, 20, 20, buttonStatus & (1<<KEY_Y_SHIFT) ? Gfx::ccc(200, 0, 0) : Gfx::ccc(160, 160, 160));

  DummyUtil::rectFill(gfxBuffer, 60, 60, 20, 20, buttonStatus & (1<<KEY_RIGHT_SHIFT) ? Gfx::ccc(200, 0, 0) : Gfx::ccc(160, 160, 160));
  DummyUtil::rectFill(gfxBuffer, 35, 85, 20, 20, buttonStatus & (1<<KEY_DOWN_SHIFT) ? Gfx::ccc(200, 0, 0) : Gfx::ccc(160, 160, 160));
  DummyUtil::rectFill(gfxBuffer, 10, 60, 20, 20, buttonStatus & (1<<KEY_LEFT_SHIFT) ? Gfx::ccc(200, 0, 0) : Gfx::ccc(160, 160, 160));
  DummyUtil::rectFill(gfxBuffer, 35, 35, 20, 20, buttonStatus & (1<<KEY_UP_SHIFT) ? Gfx::ccc(200, 0, 0) : Gfx::ccc(160, 160, 160));
  
  //Gfx::clear<u32>(gfxBuffer, Gfx::ccc(rand()%256, rand()%256, rand()%256));
}
  

static DummyCore emulator;

extern "C" CoreInterface* retrieve()
{
	return &emulator;
}

DummyCore core;