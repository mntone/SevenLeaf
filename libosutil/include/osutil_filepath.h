#ifndef _OSUTIL_FILEPATH_H
#define _OSUTIL_FILEPATH_H

#include "osutil_base.h"
#include "osutil_string.h"

#ifdef __cplusplus
extern "C" {
#endif

OSUTIL_EXPORT osutil_string_t osutil_getpath_current();
OSUTIL_EXPORT osutil_string_t osutil_getpath_executable();

OSUTIL_EXPORT osutil_string_t osutil_getpath_document();
OSUTIL_EXPORT osutil_string_t osutil_getpath_photo();
OSUTIL_EXPORT osutil_string_t osutil_getpath_screenshots();
OSUTIL_EXPORT osutil_string_t osutil_getpath_music();
OSUTIL_EXPORT osutil_string_t osutil_getpath_video();
OSUTIL_EXPORT osutil_string_t osutil_getpath_download();

OSUTIL_EXPORT osutil_string_t osutil_getpath_program_data();
OSUTIL_EXPORT osutil_string_t osutil_getpath_config();
OSUTIL_EXPORT osutil_string_t osutil_getpath_cache();
OSUTIL_EXPORT osutil_string_t osutil_getpath_temporary();

#ifdef __cplusplus
}
#endif

#endif  // _OSUTIL_FILEPATH_H
