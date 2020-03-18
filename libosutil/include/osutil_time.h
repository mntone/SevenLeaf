#ifndef _OSUTIL_TIME_H
#define _OSUTIL_TIME_H

#include "osutil_base.h"

#ifdef __cplusplus
extern "C" {
#endif

OSUTIL_EXPORT uint64_t osutil_gettime_as_nanoseconds();

OSUTIL_EXPORT void osutil_sleep(uint32_t seconds);
OSUTIL_EXPORT bool osutil_mssleep(uint32_t milliseconds);
OSUTIL_EXPORT bool osutil_nssleep(uint64_t nanoseconds);

OSUTIL_EXPORT bool osutil_wait_until_nanoseconds(uint64_t target);

#ifdef __cplusplus
}
#endif

#endif // _OSUTIL_TIME_H
