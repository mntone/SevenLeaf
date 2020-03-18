#ifndef _OSUTIL_ATOMIC_WINDOWS_H
#define _OSUTIL_ATOMIC_WINDOWS_H

#include "osutil_base.h"

#ifndef _WIN32
#include <stdatomic.h>
#else
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef volatile uint32_t osutil_atomic_uint32_t;
typedef volatile uint64_t osutil_atomic_uint64_t;
typedef volatile size_t   osutil_atomic_size_t;



#ifdef __cplusplus
}
#endif

#endif  // _OSUTIL_ATOMIC_WINDOWS_H
