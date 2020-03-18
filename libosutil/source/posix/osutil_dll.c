#include "osutil_dll.h"

#include "osutil_string.h"
#include <dlfcn.h>

module_t osutil_module_open(const char *path) {
  osutil_string_t string = { 0 };
  osutil_string_copy(&string, path);
  
#ifdef __APPLE__
  if (!osutil_string_endswith(&string, ".so") && !osutil_string_endswith(&string, ".dylib")) {
    osutil_string_join(&string, ".dylib");
  }
#else
  if (!osutil_string_endswith(&string, ".so")) {
    osutil_string_join(&string, ".so");
  }
#endif
  
  module_t module = dlopen(string.data, RTLD_LAZY);
  if (!module) {
    // TODO: error log
  }
  
  osutil_string_free(&string);
  return module;
}

symbol_t osutil_module_getsymbol(module_t module, const char *name) {
  symbol_t symbol = dlsym(module, name);
  return symbol;
}

bool osutil_module_close(module_t module) {
  int result = dlclose(module);
  return result;
}
