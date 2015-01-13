#ifndef __KEYS_H__
#define __KEYS_H__

#include "defines.h"
#include <sstream>

//TODO: for now it is header only to avoid linking with additional .cpp inside cores, let's think for a different solution
namespace gcw
{
  class Keys
  {
  public:
    static const char* nameForKey(GCWKey key)
    {
      switch (key) {
        case GCW_KEY_A: return "A";
        case GCW_KEY_B: return "B";
        case GCW_KEY_X: return "X";
        case GCW_KEY_Y: return "Y";
        case GCW_KEY_DOWN: return "\x18";
        case GCW_KEY_UP: return "\x19";
        case GCW_KEY_RIGHT: return "\x17";
        case GCW_KEY_LEFT: return "\x16";
        case GCW_KEY_L: return "L";
        case GCW_KEY_R: return "R";
        case GCW_KEY_START: return "Start";
        case GCW_KEY_SELECT: return "Select";
        case GCW_KEY_PAUSE: return "Pause";
        case GCW_KEY_HOME: return "Home";
          
        case GCW_ANALOG_DOWN: return "A\x18";
        case GCW_ANALOG_UP: return "A\x19";
        case GCW_ANALOG_RIGHT: return "A\x17";
        case GCW_ANALOG_LEFT: return "A\x16";
          
        default: return "UNKOWN";
      }
    }

    static s8 indexForKey(GCWKey key)
    {
      switch (key) {
        case GCW_KEY_A: return 0;
        case GCW_KEY_B: return 1;
        case GCW_KEY_X: return 2;
        case GCW_KEY_Y: return 3;
        case GCW_KEY_LEFT: return 4;
        case GCW_KEY_RIGHT: return 5;
        case GCW_KEY_UP: return 6;
        case GCW_KEY_DOWN: return 7;
        case GCW_KEY_L: return 8;
        case GCW_KEY_R: return 9;
        case GCW_KEY_START: return 10;
        case GCW_KEY_SELECT: return 11;
        case GCW_KEY_PAUSE: return 12;
        case GCW_KEY_HOME: return 13;
          
        case GCW_ANALOG_LEFT: return 14;
        case GCW_ANALOG_RIGHT: return 15;
        case GCW_ANALOG_UP: return 16;
        case GCW_ANALOG_DOWN: return 17;
          
        default: return SDL_INVALID_KEY;
      }
    }
    
    static GCWKey keyForMask(ButtonStatus status)
    {
      for (int i = 0; i < GCW_KEY_TOTAL_COUNT; ++i)
        if (status & (1 << i))
          return keyForIndex(i);
      
      return GCW_KEY_START;
    }
    
    static inline ButtonStatus shiftMaskForKey(GCWKey key) { return 1 << indexForKey(key); }
    
    static GCWKey keyForIndex(u8 key)
    {
      static constexpr GCWKey keys[] = {
        GCW_KEY_A, GCW_KEY_B, GCW_KEY_X, GCW_KEY_Y, GCW_KEY_LEFT, GCW_KEY_RIGHT, GCW_KEY_UP, GCW_KEY_DOWN,
        GCW_KEY_L, GCW_KEY_R, GCW_KEY_START, GCW_KEY_SELECT, GCW_KEY_PAUSE, GCW_KEY_HOME,
        GCW_ANALOG_LEFT, GCW_ANALOG_RIGHT, GCW_ANALOG_UP, GCW_ANALOG_DOWN
      };
      
      return keys[key];
    }
    
    static std::string mnemonicForKeybind(ButtonStatus status)
    {
      std::stringstream ss;
      bool first = true;
      
      for (int i = 0; i < GCW_KEY_TOTAL_COUNT; ++i)
      {
        GCWKey key = keyForIndex(i);
        
        if (status & shiftMaskForKey(key))
        {
          if (!first)
            ss << " ";
          
          ss << nameForKey(key);
          first = false;
        }
      }
      
      return ss.str();
    }
  };
}



#endif