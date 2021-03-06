#include "utils.h"

#include "sys/stat.h"
//TODO: required for ~ expansion
//#include "wordexp.h"

#include <thread>
#include <chrono>

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

Path::Path(const std::string& path) : path(path)
{
  trimEndSlash();
}

FILE* Path::open(FMode mode) const
{
  const char* smode = nullptr;
  
  switch (mode) {
    case FMode::WRITING: smode = "wb";
    case FMode::READING: smode = "rb";
    case FMode::APPENDING: smode = "wb+";
  }
  
  return fopen(path.c_str(), smode);
}

time_t Path::timeModified() const
{
  struct stat buffer;
  stat(path.c_str(), &buffer);
  return buffer.st_mtime;
}

bool Path::exists() const
{
  struct stat buffer;
  return stat(path.c_str(), &buffer) == 0;
}

bool Path::existsAsFolder() const
{
  struct stat buffer;
  return stat(path.c_str(), &buffer) == 0 && S_ISDIR(buffer.st_mode);
}

Path Path::folder() const
{
  size_t dot = path.find_last_of("/");
  
  if (dot != string::npos)
    return path.substr(0, dot);
  else
    return path;
}

std::string Path::extension() const
{
  size_t dot = path.find_last_of(".");
  
  if (dot != string::npos)
    return path.substr(dot+1, string::npos);
  else
    return string();
}

std::string Path::plainName() const
{
  size_t dot = path.find_last_of(".");
  
  if (dot != string::npos)
    return path.substr(0, dot);
  else
    return path;
}

tuple<string, string> Path::split() const
{
  size_t dot = path.find_last_of(".");

  if (dot != string::npos)
    return make_tuple(path.substr(0, dot), path.substr(dot+1, string::npos));
  else
    return make_tuple(path, "");
}

Path Path::append(const Path& path) const { return append(path.value()); }

Path Path::append(string component) const
{
  if (component.back() == '/')
    component.pop_back();
  
  if (component.front() != '/' && !isRoot())
    return Path(path + '/' + component);
  else
    return Path(path + component);
}

Path Path::removeLast() const
{
  if (!isRoot())
  {
    Path newPath = Path(*this);
    newPath.trimEndSlash();
    
    size_t slashPos = newPath.path.find_last_of('/');
    
    if (slashPos != 0)
      newPath.path = newPath.path.substr(0, slashPos);
    else
      newPath.path = "/";
    
    return newPath;
  }
  else
    return Path(*this);
}

bool Path::isRoot() const
{
  return path == "/";
}

string Path::fileInsidePath(const string& file) const
{
  return path+file;
}

vector<Path> Path::findFiles(const string& ext, bool recursive) const
{
  return Files::findFiles(path, ext, recursive);
}

vector<Path> Path::findFiles(unordered_set<string>& exts, bool recursive) const
{
  return Files::findFiles(path, exts, recursive);
}

vector<Path> Path::subfolders(bool sorted) const
{
  auto paths = Files::findSubfolders(path);
  
  if (sorted)
    sort(paths.begin(), paths.end());
  
  return paths;
}



vector<Path> Files::findFiles(const string& path, const string& ext, bool recursive)
{
  auto set = unordered_set<string>{ext};
  return findFiles(path, set, recursive);
}

// TODO: recursive not implemented
vector<Path> Files::findFiles(const string& path, unordered_set<string>& exts, bool recursive)
{
  vector<Path> files;
  
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

vector<Path> Files::findSubfolders(const string& path)
{
  vector<Path> folders;

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

bool Files::createFolder(const Path& path, bool recursive)
{
  //TODO: use expansion
  /*wordexp_t p;
  wordexp(path.c_str(), &p, 0);
  mkdir(p.we_wordv[0], 0755);
  wordfree(&p);*/
  
  if (recursive)
  {
    vector<Path> paths;
    
    Path cpath = path;
    while (!cpath.isRoot() && !cpath.existsAsFolder())
    {
      paths.push_back(cpath);
      cpath = cpath.folder();
    }
    
    auto it = paths.rbegin();
    
    for ( ; it != paths.rend(); ++it)
      mkdir(it->value().c_str(), 0755);

  }
  else if (!path.existsAsFolder())
    mkdir(path.value().c_str(), 0755);
  
  return true;
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

string Text::clipText(const string &text, s32 length, const char *filler)
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

string Text::fuzzyTimeInterval(time_t start, time_t end)
{
  auto spt = system_clock::from_time_t(start);
  auto ept = system_clock::from_time_t(end);
  
  auto delta = ept - spt;
  
  if (chrono::duration_cast<seconds>(delta).count() < 60)
    return "seconds ago";
  else if (chrono::duration_cast<minutes>(delta).count() >= 1 && chrono::duration_cast<minutes>(delta).count() <= 5)
    return to_string(chrono::duration_cast<minutes>(delta).count()) + " minutes ago";
  else if (chrono::duration_cast<hours>(delta).count() < 1)
    return "less than one hour ago";
  else if (chrono::duration_cast<hours>(delta).count() >= 1 && chrono::duration_cast<hours>(delta).count() <= 24)
    return to_string(chrono::duration_cast<hours>(delta).count()) + " hours ago";
  else if (chrono::duration_cast<hours>(delta).count() > 24 && chrono::duration_cast<hours>(delta).count() <= 48)
    return "one day ago";
  else
    return to_string(chrono::duration_cast<hours>(delta).count()/24) + " days ago";
}

string Text::dateToString(time_t timestamp)
{
  char buffer[128];
  strftime(buffer, 128, "%d/%m/%y %H:%M", std::localtime(&timestamp));
  return string(buffer);
}

