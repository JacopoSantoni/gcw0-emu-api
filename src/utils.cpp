#include "utils.h"

#include "sys/stat.h"

#include <thread>

using namespace std;
using namespace std::chrono;
using namespace gcw;

/*
Path(std::string path) : path(path) { }
std::string &string() { return path; }
void append(std::string component);
void removeLast();
bool isRoot();


std::string fileInsidePath(std::string file);
std::vector<std::string> findFiled(std::string ext, bool recursive);
std::vector<std::string> findFiles(std::unordered_set<std::string> exts, bool recursive);
std::vector<std::string> subfolders();*/

Path::Path(const char *path): path(path)
{
  trimEndSlash();
}

Path::Path(std::string path) : path(path)
{
  trimEndSlash();
}

void Path::append(string component)
{
  if (component.back() == '/')
    component.pop_back();
  
  if (component.front() != '/' && !isRoot())
    path += '/' + component;
  else
    path += component;
}

void Path::removeLast()
{
  if (path != "/")
  {
    trimEndSlash();
    
    size_t slashPos = path.find_last_of('/');
    
    if (slashPos != 0)
      path = path.substr(0, slashPos);
    else
      path = "/";
  }
}

bool Path::isRoot() const
{
  return path == "/";
}

string Path::fileInsidePath(string file) const
{
  return path+file;
}

vector<string> Path::findFiles(string ext, bool recursive) const
{
  return Files::findFiles(path, ext, recursive);
}

vector<string> Path::findFiles(unordered_set<string> exts, bool recursive) const
{
  return Files::findFiles(path, exts, recursive);
}

vector<string> Path::subfolders() const
{
  return Files::findSubfolders(path);
}



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
  if ((dir = opendir (path.c_str())) != NULL)
  {
    while ((ent = readdir (dir)) != NULL)
    {
      stat((path+"/"+ent->d_name).c_str(), &stats);
      if (S_ISDIR(stats.st_mode) && strcmp(ent->d_name, ".") != 0 && (path != "/" || strcmp(ent->d_name, "..") != 0))
        folders.push_back(ent->d_name);
    }
  }
  else if (path != "/")
    folders.push_back("..");
  
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

#pragma mark Text

string Text::clipText(string &text, s32 length, const char *filler)
{
  bool reversed = false;
  if (length < 0)
  {
    length = -length;
    reversed = true;
  }
  
  if (text.length() < length)
    return text;
  else
  {
    string clipped = reversed ? text.substr(text.length()-length, string::npos) : text.substr(0,length);
    
    if (filler)
      return reversed ? filler + clipped : clipped + filler;
    else
      return clipped;
  }
}