#ifndef _DEFINES_H_
#define _DEFINES_H_

#include <SDL.h>
#include <assert.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;

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
  GCWKEY_LEFT = SDLK_LEFT,
  GCWKEY_RIGHT = SDLK_RIGHT,
  GCWKEY_UP = SDLK_UP,
  GCWKEY_DOWN = SDLK_DOWN,
  
  GCWKEY_X = SDLK_LSHIFT,
  GCWKEY_Y = SDLK_SPACE,
  GCWKEY_A = SDLK_LCTRL,
  GCWKEY_B = SDLK_LALT,
  
  GCWKEY_L = SDLK_TAB,
  GCWKEY_R = SDLK_BACKSPACE,
  
  GCWKEY_SELECT = SDLK_ESCAPE,
  GCWKEY_START = SDLK_RETURN,
};

const s8 SDL_INVALID_KEY = -1;
const int GCWKEY_AMOUNT = 12;

const s16 GCW_ANALOG_MIN = 0;
const s16 GCW_ANALOG_MAX = 32768;

enum GCWAnalogAxis
{
  GCW_ANALOG_AXIS_X = 0x01,
  GCW_ANALOG_AXIS_Y = 0x02,
  GCW_ANALOG_AXIS_BOTH = GCW_ANALOG_AXIS_X | GCW_ANALOG_AXIS_Y
};

typedef u32 ButtonStatus;


#endif