#ifndef _OSUTIL_THREAD_H
#define _OSUTIL_THREAD_H

#include "osutil_base.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _WIN32
#ifndef _WINDEF_
typedef unsigned long DWORD;
#endif
typedef DWORD process_identifier_t;
typedef DWORD thread_identifier_t;
#elif __APPLE__
typedef int process_identifier_t;
typedef int thread_identifier_t;
#endif

OSUTIL_EXPORT process_identifier_t osutil_get_process_identifier();
OSUTIL_EXPORT thread_identifier_t osutil_get_thread_identifier();

OSUTIL_EXPORT void osutil_set_thread_name(const char *thread_name);

#ifdef __cplusplus
}
#endif

#endif // _OSUTIL_THREAD_H
