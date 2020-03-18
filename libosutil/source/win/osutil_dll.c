#include "osutil_dll.h"

//module_t osutil_dll_open() {}

symbol_t osutil_module_getsymbol(module_t module, const char *name) {
  FARPROC symbol = GetProcAddress(module, name);
  return symbol;
}

bool osutil_module_close(module_t module) {
  BOOL result = FreeLibrary(module);
  return result;
}
