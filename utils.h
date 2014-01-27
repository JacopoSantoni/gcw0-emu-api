#ifndef _UTILS_H_
#define _UTILS_H_

#include <dirent.h>

#include <vector>
#include <string>

class Files
{
	public:
		static std::vector<std::string> findFiles(std::string path, const char *ext);

};

#endif
