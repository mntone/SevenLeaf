#ifndef _OSUTIL_DLL_H
#define _OSUTIL_DLL_H

#include "osutil_base.h"

#ifdef _WIN32
#ifndef STRICT
#define STRICT
#endif
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _WIN32
typedef HMODULE module_t;
typedef FARPROC symbol_t;
#else
typedef intptr_t module_t;
typedef intptr_t symbol_t;
#endif

OSUTIL_EXPORT module_t osutil_module_open(const char *path);
OSUTIL_EXPORT symbol_t osutil_module_getsymbol(module_t module, const char *name);
OSUTIL_EXPORT bool osutil_module_close(module_t module);

#ifdef __cplusplus
}
#endif

#endif  // _OSUTIL_DLL_H
