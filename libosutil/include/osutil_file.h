#ifndef _OSUTIL_FILE_H
#define _OSUTIL_FILE_H

#include "osutil_base.h"

#ifdef __cplusplus
extern "C" {
#endif

OSUTIL_EXPORT bool osutil_path_exists(const char *path);
OSUTIL_EXPORT bool osutil_directory_exists(const char *path);

#ifdef __cplusplus
}
#endif

#endif  // _OSUTIL_FILE_H
