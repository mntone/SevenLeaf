#ifndef _OSUTIL_BASE_H
#define _OSUTIL_BASE_H

#include <stdint.h>
#include <stdbool.h>

// clang-format off
#ifdef _WIN32
#  ifdef libosutil_EXPORTS
#    define OSUTIL_EXPORT __declspec(dllexport)
#  else
#    define OSUTIL_EXPORT __declspec(dllimport)
#  endif
#else
#  ifdef libosutil_EXPORTS
#    define OSUTIL_EXPORT  __attribute__((visibility("default")))
#  else
#    define OSUTIL_EXPORT
#  endif
#endif
// clang-format on

#endif // _OSUTIL_BASE_H
