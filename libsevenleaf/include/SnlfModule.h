#ifndef _SNLF_MODULE_H
#define _SNLF_MODULE_H

#include "SnlfCore.h"

struct _SnlfLibraryDescriptor {
  uint64_t compositorVersion;
  CpsrDriverType compositorDriverType;
  uint64_t sevenleafAPIVersion;
  uint64_t sevenleafLibraryVersion;
  uint64_t _reserved[4];
};
typedef struct _SnlfLibraryDescriptor SnlfLibraryDescriptor;

struct _SnlfModuleDescriptor {
  const char identifier[64];
  const char version[24];
  uint64_t   comparableVersion;
  const char authorName[32];
  uint8_t _reserved[64];
};
typedef struct _SnlfModuleDescriptor SnlfModuleDescriptor;

#define SNLF_MODULE_GET_INFO "SnlfModuleGetInfo"
#define SNLF_MODULE_CAN_LOAD "SnlfModuleCanLoad"
#define SNLF_MODULE_LOAD     "SnlfModuleLoad"
#define SNLF_MODULE_UNLOAD   "SnlfModuleUnload"

// ---
// Definitions for Plugins
// ---
#ifndef libsevenleaf_EXPORTS
// clang-format on
#ifdef _WIN32
#  ifdef __cplusplus
#    define SNLF_MODULE_EXPORT extern "C" __declspec(dllexport)
#  else
#    define SNLF_MODULE_EXPORT extern __declspec(dllexport)
#  endif
#else
#  ifdef __cplusplus
#    define SNLF_MODULE_EXPORT extern "C" __attribute__((visibility("default")))
#  else
#    define SNLF_MODULE_EXPORT extern __attribute__((visibility("default")))
#  endif
#endif
// clang-format off

SNLF_MODULE_EXPORT void SnlfModuleGetInfo(SnlfModuleDescriptor *descriptor);
SNLF_MODULE_EXPORT bool SnlfModuleCanLoad(const SnlfLibraryDescriptor *libraryDescriptor);
SNLF_MODULE_EXPORT intptr_t SnlfModuleLoad(SnlfCoreRef core);
SNLF_MODULE_EXPORT bool SnlfModuleUnload(intptr_t context);
#endif

#endif // _SNLF_MODULE_H
