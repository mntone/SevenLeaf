#include "osutil_time.h"

#include <unistd.h>
#include <time.h>
#include <string.h>

void osutil_sleep(uint32_t seconds) {
  sleep(seconds);
}

bool osutil_mssleep(uint32_t milliseconds) {
  return usleep(1000 * milliseconds);
}

bool osutil_nssleep(uint64_t nanoseconds) {
  struct timespec request = { 0 };
  struct timespec remain  = { 0 };

  request.tv_sec = nanoseconds / 1000000000;
  request.tv_nsec = nanoseconds % 1000000000;
  while (nanosleep(&request, &remain)) {
    request = remain;
    memset(&remain, 0, sizeof(struct timespec));
  }
  return false;
}
