#ifndef _DEFINES_H_
#define _DEFINES_H_


#include <SDL.h>
#include <assert.h>

#define _VERBOSE_

#ifdef _VERBOSE_
#include <cstdio>
#endif

#ifdef _VERBOSE_
#define LOG(...) printf(__VA_ARGS__);
#else
#define LOG(...) do { } while (false)
#endif

#include <unordered_map>

#include "../systems/systems.h"

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;

const s8 SDL_INVALID_KEY = -1;
const int GCW_KEY_COUNT = 14;
const int GCW_ANALOG_COUNT = 4;
const int GCW_ANALOG_MAP_SHIFT = GCW_KEY_COUNT;
const int GCW_KEY_TOTAL_COUNT = GCW_KEY_COUNT + GCW_ANALOG_COUNT;

enum GCWKey : s16
{
  GCW_KEY_LEFT = SDLK_LEFT,
  GCW_KEY_RIGHT = SDLK_RIGHT,
  GCW_KEY_UP = SDLK_UP,
  GCW_KEY_DOWN = SDLK_DOWN,
  
  GCW_KEY_X = SDLK_LSHIFT,
  GCW_KEY_Y = SDLK_SPACE,
  GCW_KEY_A = SDLK_LCTRL,
  GCW_KEY_B = SDLK_LALT,
  
  GCW_KEY_L = SDLK_TAB,
  GCW_KEY_R = SDLK_BACKSPACE,
  
  GCW_KEY_SELECT = SDLK_ESCAPE,
  GCW_KEY_START = SDLK_RETURN,
  
  GCW_KEY_PAUSE = SDLK_PAUSE,
  GCW_KEY_HOME = SDLK_HOME,
  
  GCW_ANALOG_LEFT = SDLK_a,
  GCW_ANALOG_RIGHT = SDLK_d,
  GCW_ANALOG_UP = SDLK_w,
  GCW_ANALOG_DOWN = SDLK_s
};
  
#define MENU_ACTION_BUTTON (GCW_KEY_A)
#define MENU_BACK_BUTTON (GCW_KEY_B)

const u8 GCW_ANALOG_LEFT_INDEX = GCW_KEY_COUNT;
const u8 GCW_ANALOG_RIGHT_INDEX = GCW_KEY_COUNT+1;
const u8 GCW_ANALOG_UP_INDEX = GCW_KEY_COUNT+2;
const u8 GCW_ANALOG_DOWN_INDEX = GCW_KEY_COUNT+3;

const s32 GCW_ANALOG_MIN = 0;
const s32 GCW_ANALOG_MAX = 32768;
  
const s16 GWC_ANALOG_DEAD_ZONE_DEFAULT_MIN = 500;
const s16 GCW_ANALOG_DEAD_ZONE_DEFAULT_MAX = 32768;
  
const u8 GCW_ANALOG_AXIS_X = 0;
const u8 GCW_ANALOG_AXIS_Y = 1;

enum AnalogMode
{
  GCW_ANALOG_NONE,
  GCW_ANALOG_REAL_MODE,
  GCW_ANALOG_DIGITAL_MODE
};
  
struct AnalogStatus
{
  float x;
  float y;
  
  //AnalogStatus() : x(0.0f), y(0.0f) { } TODO: commented
};


struct AnalogDeadZone
{
  float min;
  float max;
  
  AnalogDeadZone() : min(static_cast<float>(GWC_ANALOG_DEAD_ZONE_DEFAULT_MIN)/GCW_ANALOG_MAX), max(static_cast<float>(GCW_ANALOG_DEAD_ZONE_DEFAULT_MAX)/GCW_ANALOG_MAX) { }
};
  
  

typedef u32 ButtonStatus;


const u16 MAX_SAVE_SLOTS = 10;
typedef u32 SaveSlot;

struct CoreIdentifier
{
  std::string ident;
  std::string version;
  
  CoreIdentifier() = default;
  CoreIdentifier(const std::string& ident, const std::string& version) : ident(ident), version(version) { }
  
  bool isValid() const { return !ident.empty(); }
  operator bool() const { return isValid(); }
  bool operator==(const CoreIdentifier& other) const { return ident == other.ident && version == other.version; }
  bool operator!=(const CoreIdentifier& other) const { return !(*this == other); }
  
  const std::string identifier() const { return ident + "-" + version; }
};

enum GfxBufferFormat
{
  FORMAT_XRGB888,
  FORMAT_RGB565,
  FORMAT_RGBA5551,
  FORMAT_RGBA8888,
};

struct GfxBufferSpec
{
  u16 width;
  u16 height;
  GfxBufferFormat format;
};

struct GfxBuffer
{
  void *data;
  //u16 pitch;
  u16 width;
  u16 height;
  
  GfxBuffer() : data(nullptr), /*pitch(0),*/ width(0), height(0) { }
  //void allocate(u16 width, u16 height, u16 pitch)
  
  void allocate(const GfxBufferSpec& spec)
  {
    delete [] static_cast<u32*>(data);
    
    u32 bufferSize = 0;
    //pitch = spec.width;
    width = spec.width;
    height = spec.height;
    
    switch (spec.format)
    {
      case FORMAT_XRGB888: bufferSize = width*height; break;
      case FORMAT_RGB565: bufferSize = width*height/2; break;
    }
    
    data = new u32[bufferSize];
    
  }
  
  template <typename T>
  inline T* getData() const { return static_cast<T*>(data); }
  
  void release()
  {
    delete [] static_cast<u32*>(data);
    data = nullptr;
  }
  
  //virtual ~GfxBuffer() { delete [] data; }
  
  //~GfxBuffer() { if (data) delete [] data; }
};

struct Offset
{
  u16 x;
  u16 y;
  
  Offset() = default;
  Offset(u16 x, u16 y) : x(x), y(y) { }
};

typedef Offset Size;


struct SfxAudioSpec
{
  u32 bufferElementSize;
  u32 bufferSize;

  u16 sampleRate;
  
  SfxAudioSpec(u32 bufferElementSize, u32 bufferSize, u16 sampleRate) : bufferElementSize(bufferElementSize), bufferSize(bufferSize), sampleRate(sampleRate) { }
  
  u32 totalBufferSize() const { return bufferSize * bufferElementSize; }
};

struct CorePreferences
{
  std::string scaler;
  std::unordered_map<std::string, std::string> settings;
  
  
  const std::string& valueForSetting(const std::string& ident) const { return settings.find(ident)->second; }
  void setValueForSetting(const std::string& ident, const std::string& value) { settings[ident] = value; }
};


#endif
