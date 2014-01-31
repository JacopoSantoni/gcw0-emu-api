#include "utils.h"

#include <thread>

using namespace std;
using namespace std::chrono;
using namespace gcw;

vector<string> Files::findFiles(string path, string ext)
{
  return findFiles(path, vector<string>{ext});
}

vector<string> Files::findFiles(string path, vector<string> exts)
{
  vector<string> files;
  
  DIR *dir;
  struct dirent *ent;
  if ((dir = opendir (path.c_str())) != NULL) {
    while ((ent = readdir (dir)) != NULL) {
      for (string &ext : exts)
      {
        if (strncmp(ent->d_name + strlen(ent->d_name) - (ext.length()), ext.c_str(), ext.length()) == 0)
        {
          files.push_back(ent->d_name);
          break;
        }
      }
    }
    closedir (dir);
  } else {
    /* could not open directory */
    //perror ("");
  }
  
  return files;
}


#pragma mark Timer

void Timer::setFrameRate(float rate)
{
  LOG("setting framerate to %f\n", rate);
  totalFrames = 0;
  ticksForFrame = microseconds(static_cast<u32>(1000000 / rate));
  base = clock.now();
}

void Timer::frameRateDelay()
{
  totalFrames++;
  
  time_point<steady_clock> current = clock.now();
  time_point<steady_clock> target = base + (ticksForFrame * totalFrames);
  
  if (current <= target)
  {
    /*microseconds delay = duration_cast<microseconds>(target-current);
    microseconds spent = duration_cast<microseconds>(current - (target - ticksForFrame));
    microseconds total = delay + spent;
    
    LOG("delay: %lldus - spent: %lldus - total: %lldus\n", delay.count(), spent.count(), total.count());*/
    this_thread::sleep_for(target-current);
  }
  else
  {
    base = current;
    totalFrames = 0;
    // frame required more time than requested
  }
}