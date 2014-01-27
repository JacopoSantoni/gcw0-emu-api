#include "utils.h"

using namespace std;

vector<string> Files::findFiles(string path, const char *ext)
{
  vector<string> files;

  DIR *dir;
  struct dirent *ent;
  if ((dir = opendir (path.c_str())) != NULL) {
    while ((ent = readdir (dir)) != NULL) {
      if (strncmp(ent->d_name + strlen(ent->d_name) - (strlen(ext)), ext, strlen(ext)) == 0)
      {
        files.push_back(ent->d_name);
        //printf ("adding %s\n", ent->d_name);

      }
    }
    closedir (dir);
  } else {
    /* could not open directory */
    //perror ("");
  }

  return files;
}
