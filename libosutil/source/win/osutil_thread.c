#include "osutil_thread.h"

#ifndef STRICT
#define STRICT
#endif
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

process_identifier_t osutil_get_process_identifier() {
  return GetCurrentProcessId();
}

thread_identifier_t osutil_get_thread_identifier() {
  return GetCurrentThreadId();
}
