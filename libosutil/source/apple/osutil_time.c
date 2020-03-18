#include "osutil_time.h"
#include "osutil_uint128_t.h"

#include <mach/mach_time.h>
#include <unistd.h>

static mach_timebase_info_data_t timebase_info = { 0, 0 };
static bool unity = false;

static inline void get_timebase_info_if_needed() {
  if (timebase_info.denom == 0) {
    mach_timebase_info(&timebase_info);
    unity = timebase_info.numer == timebase_info.denom;
  }
}

uint64_t osutil_gettime_as_nanoseconds() {
  get_timebase_info_if_needed();
  
  if (unity) {
    return mach_absolute_time();
  } else {
    return (uint64_t)((((double)timebase_info.numer) / timebase_info.denom) * mach_absolute_time());
  }
}

void osutil_sleep(uint32_t seconds) {
  sleep(seconds);
}

bool osutil_mssleep(uint32_t milliseconds) {
  return usleep(1000 * milliseconds);
}

bool osutil_nssleep(uint64_t nanoseconds) {
  get_timebase_info_if_needed();
  
  uint64_t current = mach_absolute_time();
  uint64_t end;
  if (unity) {
    end = current + nanoseconds;
  } else {
    // TODO: muldiv128
    end = current + osutil_udiv128(osutil_umul64x64(timebase_info.denom, nanoseconds), timebase_info.numer);
  }
  
  kern_return_t ret;
  do {
    ret = mach_wait_until(end);
  } while (ret == KERN_ABORTED && (current = mach_absolute_time() >= end));
  return ret != KERN_SUCCESS;
}

bool osutil_wait_until_nanoseconds(uint64_t target) {
  /*uint64_t current = osutil_gettime_as_nanoseconds();
  if (current >= target) {
    return false;
  }
  
  uint64_t b = current;
  uint64_t microseconds = (target - current) / 1000;
  if (microseconds > 999) {
    printf("wait during %llu us, t-c: %llu\n", microseconds, (target - current));
    usleep(microseconds);
  }
  while (current < target) {
    _mm_pause();
    current = osutil_gettime_as_nanoseconds();
  }
  uint64_t e = osutil_gettime_as_nanoseconds();
  printf("%llu ns, %d us\n", e - b, (e - b) / 1000);
  return false;*/
  
  /*uint64_t current = osutil_gettime_as_nanoseconds();
  if (target < current) {
      return true;
  }
  target -= current;

  struct timespec request;
  request.tv_sec = target / 1000000000;
  request.tv_nsec = target % 1000000000;
  
  struct timespec remain = {0};
  while (nanosleep(&request, &remain)) {
      request = remain;
      memset(&remain, 0, sizeof(remain));
  }
  return false;*/
  get_timebase_info_if_needed();
  
  if (!unity) {
    target = osutil_udiv128(osutil_umul64x64(timebase_info.denom, target), timebase_info.numer);
  }
  
  kern_return_t ret;
  do {
    ret = mach_wait_until(target);
  } while (ret == KERN_ABORTED && (osutil_gettime_as_nanoseconds() < target));
  return ret != KERN_SUCCESS;
}
