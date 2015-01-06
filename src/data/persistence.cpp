#include "persistence.h"

using namespace gcw;

#ifdef __APPLE__
const Path Persistence::ROOT_PATH = Path("/Users/jack/Documents/Dev/github/gcw0-emu-api/xcode/root");
#else
const Path Persistence::ROOT_PATH = Path("/");
#endif
