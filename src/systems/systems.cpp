#include "systems.h"

using namespace std;
using namespace gcw;

vector<System::Spec> System::specs = {
  System::Spec(System::Type::UNCATEGORISED, "uncat", "Uncategorized", {}),
  System::Spec(System::Type::SUPER_NINTENDO, "snes", "Super Nintendo", {"smc","fig"}),
  System::Spec(System::Type::GAME_BOY, "gb", "GameBoy", {"gb"}),
  System::Spec(System::Type::GAME_BOY_COLOR, "gbc", "GameBoy Color", {"gbc"}),
  System::Spec(System::Type::GAME_BOY_ADVANCE, "gba", "GameBoy Advance", {"gba"}),
  
  System::Spec(System::Type::GAME_GEAR, "gg", "Game Gear", {"gg"}),
  System::Spec(System::Type::SEGA_MASTER_SYSTEM, "sms", "Sega Master System", {"sms"})
};
