#include "osutil_file.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

bool osutil_path_exists(const char *path) {
  return access(path, F_OK) == 0;
}

bool osutil_directory_exists(const char *path) {
  struct stat info;
  if (stat(path, &info) != 0) {
    return false;
  }
  return S_ISDIR(info.st_mode);
}
