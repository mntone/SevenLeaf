#include "osutil_system.h"

#include <sys/sysctl.h>

#define GET_INFO_FROM_SYSCTLBYNAME(__TYPE__, __NAME__) \
  __TYPE__ num = 0; \
  size_t len = sizeof(__TYPE__); \
  int ret = sysctlbyname(__NAME__, &num, &len, NULL, 0); \
  if (ret) { \
    return -1; \
  } \
  return num

int32_t osutil_get_physical_cores() {
  GET_INFO_FROM_SYSCTLBYNAME(int32_t, "hw.physicalcpu");
}

int32_t osutil_get_logical_cores() {
  GET_INFO_FROM_SYSCTLBYNAME(int32_t, "hw.logicalcpu");
}

int64_t osutil_get_l1d_cachesize() {
  GET_INFO_FROM_SYSCTLBYNAME(int64_t, "hw.l1dcachesize");
}

int64_t osutil_get_l1i_cachesize() {
  GET_INFO_FROM_SYSCTLBYNAME(int64_t, "hw.l1icachesize");
}

int64_t osutil_get_l2_cachesize() {
  GET_INFO_FROM_SYSCTLBYNAME(int64_t, "hw.l2cachesize");
}

int64_t osutil_get_l3_cachesize() {
  GET_INFO_FROM_SYSCTLBYNAME(int64_t, "hw.l3cachesize");
}

int64_t osutil_get_memsize() {
  GET_INFO_FROM_SYSCTLBYNAME(int64_t, "hw.memsize");
}

int64_t osutil_get_pagesize() {
  GET_INFO_FROM_SYSCTLBYNAME(int64_t, "hw.pagesize");
}

#undef GET_INFO_FROM_SYSCTLBYNAME
