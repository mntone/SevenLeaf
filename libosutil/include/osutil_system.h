#ifndef _OSUTIL_SYSTEM_H
#define _OSUTIL_SYSTEM_H

#include "osutil_base.h"

#ifdef __cplusplus
extern "C" {
#endif

OSUTIL_EXPORT int32_t osutil_get_physical_cores();
OSUTIL_EXPORT int32_t osutil_get_logical_cores();

OSUTIL_EXPORT int64_t osutil_get_l1d_cachesize();
OSUTIL_EXPORT int64_t osutil_get_l1i_cachesize();
OSUTIL_EXPORT int64_t osutil_get_l2_cachesize();
OSUTIL_EXPORT int64_t osutil_get_l3_cachesize();

OSUTIL_EXPORT int64_t osutil_get_memsize();
OSUTIL_EXPORT int64_t osutil_get_pagesize();

// Battery
OSUTIL_EXPORT bool osutil_is_battery_status_present();

#ifdef __cplusplus
}
#endif

#endif // _OSUTIL_SYSTEM_H
