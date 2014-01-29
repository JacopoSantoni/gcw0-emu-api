#include "utils.h"

using namespace std;
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
