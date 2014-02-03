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
  KEY_DOWN_SHIFT = 7,
  
  KEY_L_SHIFT = 8,
  KEY_R_SHIFT = 9,
  
  KEY_START_SHIFT = 10,
  KEY_SELECT_SHIFT = 11
  
};
  
class DummyUtil
{
  public:
  template<typename T>
  static void rectFill(GfxBuffer buffer, int x, int y, int w, int h, T color)
  {
    T* data = reinterpret_cast<T*>(buffer.data);
    
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
      
      EnumSet<int32_t> frameSkip = {EnumValue<int32_t>("AUTO",-1),EnumValue<int32_t>("0",0),EnumValue<int32_t>("1",1),EnumValue<int32_t>("2",2),EnumValue<int32_t>("3",3),EnumValue<int32_t>("4",4),EnumValue<int32_t>("5",5)};
      registerSetting(new EnumSetting<int32_t>("Frameskip", "frameskip", frameSkip, frameSkip[0]));
      
      registerSetting(new BoolSetting("Transparency", "transparency", false));
      registerSetting(new BoolSetting("Show FPS", "show-fps", false));
            
      registerButton(ButtonSetting("A", GCW_KEY_A, KEY_A_SHIFT, true));
      registerButton(ButtonSetting("B", GCW_KEY_B, KEY_B_SHIFT, true));
      registerButton(ButtonSetting("X", GCW_KEY_X, KEY_X_SHIFT, true));
      registerButton(ButtonSetting("Y", GCW_KEY_Y, KEY_Y_SHIFT, true));
      registerButton(ButtonSetting("L", GCW_KEY_L, KEY_L_SHIFT, true));
      registerButton(ButtonSetting("R", GCW_KEY_R, KEY_R_SHIFT, true));
      registerButton(ButtonSetting("Start", GCW_KEY_START, KEY_START_SHIFT, true));
      registerButton(ButtonSetting("Select", GCW_KEY_SELECT, KEY_SELECT_SHIFT, true));
      registerButton(ButtonSetting("Up", GCW_KEY_UP, KEY_UP_SHIFT, true));
      registerButton(ButtonSetting("Right", GCW_KEY_RIGHT, KEY_RIGHT_SHIFT, true));
      registerButton(ButtonSetting("Left", GCW_KEY_LEFT, KEY_LEFT_SHIFT, true));
      registerButton(ButtonSetting("Down", GCW_KEY_DOWN, KEY_DOWN_SHIFT, true));
      
      // this enables a joypad direction as a digital button
      /*registerButton(ButtonSetting("LeftA", GCW_ANALOG_LEFT, KEY_LEFT_SHIFT, true));
      registerButton(ButtonSetting("RightA", GCW_ANALOG_RIGHT, KEY_RIGHT_SHIFT, true));
      registerButton(ButtonSetting("UpA", GCW_ANALOG_UP, KEY_UP_SHIFT, true));
      registerButton(ButtonSetting("DownA", GCW_ANALOG_DOWN, KEY_DOWN_SHIFT, true));*/
      
      // this instead enables the joypad for normal analog use, of course they can't be both active!
      enableNormalAnalogJoypad();
      
      
      setAnalogDeadZone(0.20f, 1.0f);
      
      setGfxFormat(240, 160, FORMAT_565);

    }

    virtual void emulationFrame();
    virtual void run(int argc, char **argv) { /*mainEntry(argc, argv);*/ }
};
  
void DummyCore::emulationFrame()
{
  Gfx::clear<u16>(gfxBuffer, Gfx::ccc<u16>(220,220,220));
  
  u16 red = Gfx::ccc<u16>(200,0,0);
  u16 grey = Gfx::ccc<u16>(160,160,160);

  DummyUtil::rectFill(gfxBuffer, 210, 60, 20, 20, buttonStatus & (1<<KEY_A_SHIFT) ? red : grey);
  DummyUtil::rectFill(gfxBuffer, 185, 85, 20, 20, buttonStatus & (1<<KEY_B_SHIFT) ? red : grey);
  DummyUtil::rectFill(gfxBuffer, 160, 60, 20, 20, buttonStatus & (1<<KEY_X_SHIFT) ? red : grey);
  DummyUtil::rectFill(gfxBuffer, 185, 35, 20, 20, buttonStatus & (1<<KEY_Y_SHIFT) ? red : grey);

  DummyUtil::rectFill(gfxBuffer, 60, 60, 20, 20, buttonStatus & (1<<KEY_RIGHT_SHIFT) ? red : grey);
  DummyUtil::rectFill(gfxBuffer, 35, 85, 20, 20, buttonStatus & (1<<KEY_DOWN_SHIFT) ? red : grey);
  DummyUtil::rectFill(gfxBuffer, 10, 60, 20, 20, buttonStatus & (1<<KEY_LEFT_SHIFT) ? red : grey);
  DummyUtil::rectFill(gfxBuffer, 35, 35, 20, 20, buttonStatus & (1<<KEY_UP_SHIFT) ? red : grey);
  
  DummyUtil::rectFill(gfxBuffer, 10, 10, 30, 10, buttonStatus & (1<<KEY_L_SHIFT) ? red : grey);
  DummyUtil::rectFill(gfxBuffer, 200, 10, 30, 10, buttonStatus & (1<<KEY_R_SHIFT) ? red : grey);
  
  DummyUtil::rectFill(gfxBuffer, 200, 120, 30, 10, buttonStatus & (1<<KEY_SELECT_SHIFT) ? red : grey);
  DummyUtil::rectFill(gfxBuffer, 200, 140, 30, 10, buttonStatus & (1<<KEY_START_SHIFT) ? red : grey);
  
  int size = 10;
  int cx = 45-size/2, cy = 140-size/2;
  int range = 16;
  
  DummyUtil::rectFill<u16>(gfxBuffer, cx - range, cy - range, 2*range + size, 2*range + size, grey );
  DummyUtil::rectFill<u16>(gfxBuffer, cx+range*analogStatus.x, cy+range*analogStatus.y, 10, 10, red );
}
  

static DummyCore emulator;

extern "C" CoreInterface* retrieve()
{
	return &emulator;
}

DummyCore core;
