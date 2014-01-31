#ifndef _DEFINES_H_
#define _DEFINES_H_

#include <SDL.h>
#include <assert.h>

#define _VERBOSE_

#ifdef _VERBOSE_
#include <cstdio>
#endif

#ifdef _VERBOSE_
#define LOG(args...) printf(args);
#elif
#define LOG(args...) do { } while (false)
#endif

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t s8;
typedef int16_t s16;

enum ConsoleType : u16
{
  CONSOLE_UNSPECIFIED,
  
  CONSOLE_GAME_BOY,
  CONSOLE_SUPER_NINTENDO
};

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
};
  
enum GCWAnalog : s16
{
  GCW_ANALOG_LEFT = 0,
  GCW_ANALOG_RIGHT,
  GCW_ANALOG_UP,
  GCW_ANALOG_DOWN
};
  


const s8 SDL_INVALID_KEY = -1;
const int GCW_KEY_COUNT = 12;
const int GCW_ANALOG_COUNT = 4;


const s16 GCW_ANALOG_MIN = 0;
const s16 GCW_ANALOG_MAX = 32768;
  
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
};
  

struct AnalogDeadZone
{
  float min;
  float max;
  
  AnalogDeadZone() : min(static_cast<float>(GWC_ANALOG_DEAD_ZONE_DEFAULT_MIN)/GCW_ANALOG_MAX), max(static_cast<float>(GCW_ANALOG_DEAD_ZONE_DEFAULT_MAX)/GCW_ANALOG_MAX) { }
};
  
  

typedef u32 ButtonStatus;


#endif