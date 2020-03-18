#include "osutil_time.h"

#ifndef STRICT
#define STRICT
#endif
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

static inline uint64_t osutil_get_clock_frequency() {
  static LARGE_INTEGER clock_frequency = { 0 };
  if (clock_frequency.QuadPart == 0) {
    QueryPerformanceFrequency(&clock_frequency);
  }
  return clock_frequency.QuadPart;
}

uint64_t osutil_gettime_as_nanoseconds() {
  const uint64_t frequency = osutil_get_clock_frequency();
  LARGE_INTEGER current = { 0 };
  QueryPerformanceCounter(&current);
  return (uint64_t)(1000000000.0 * (double)current.QuadPart / (double)frequency);
}

void osutil_sleep(uint32_t seconds) {
  Sleep(1000 * seconds);
}

bool osutil_mssleep(uint32_t milliseconds) {
  Sleep(milliseconds);
  return false;
}

bool osutil_nssleep(uint64_t nanoseconds) {
  uint64_t current = osutil_gettime_as_nanoseconds();
  uint64_t target = current + nanoseconds;
  if (current >= target) {
    return true;
  }

  uint64_t milliseconds = (target - current) / 1000000;
  if (milliseconds > 0) {
    Sleep((DWORD)milliseconds);
  }
  while (current < target) {
    _mm_pause();
    current = osutil_gettime_as_nanoseconds();
  }
  return false;
}

bool osutil_wait_until_nanoseconds(uint64_t target) {
  uint64_t current = osutil_gettime_as_nanoseconds();
  if (current >= target) {
    return true;
  }

  uint64_t milliseconds = (target - current) / 1000000;
  if (milliseconds > 0) {
    Sleep((DWORD)milliseconds);
  }
  while (current < target) {
    _mm_pause();
    current = osutil_gettime_as_nanoseconds();
  }
  return false;
}
