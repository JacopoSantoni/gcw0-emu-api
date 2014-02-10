#include "utils.h"

#include "sys/stat.h"

#include <thread>

using namespace std;
using namespace std::chrono;
using namespace gcw;

vector<string> Files::findFiles(string path, string ext, bool recursive)
{
  return findFiles(path, unordered_set<string>{ext}, recursive);
}

// TODO: recursive not implemented
vector<string> Files::findFiles(string path, unordered_set<string> exts, bool recursive)
{
  vector<string> files;
  
  DIR *dir;
  struct dirent *ent;
  if ((dir = opendir (path.c_str())) != NULL) {
    while ((ent = readdir (dir)) != NULL) {
      for (const string &ext : exts)
      {
        string rext = "."+ext;
        if (strncmp(ent->d_name + strlen(ent->d_name) - (rext.length()), rext.c_str(), rext.length()) == 0)
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

std::vector<std::string> Files::findSubfolders(std::string path)
{
  vector<string> folders;

  DIR *dir;
  struct stat stats;
  struct dirent *ent;
  if ((dir = opendir (path.c_str())) != NULL) {
    while ((ent = readdir (dir)) != NULL) {
      stat(ent->d_name, &stats);
      if (S_ISDIR(stats.st_mode) && strcmp(ent->d_name, ".") != 0)
        folders.push_back(ent->d_name);
    }
  }
  
  return folders;
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