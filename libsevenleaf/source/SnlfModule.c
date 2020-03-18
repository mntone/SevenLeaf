#include "SnlfModule.h"
#include "SnlfCore+Private.h"

#define SnlfSymbolLoadFailedError(__SYMBOL_NAME__) SnlfAssertFormat("Symbol \"%s\" load failed.", #__SYMBOL_NAME__)

struct _SnlfModule {
  SnlfCoreRef core;
  const char *path;
  module_t module;
  bool binded;
  intptr_t context;
  
  // Symbols
  void (*getInfo)(SnlfModuleDescriptor *);
  bool (*canBind)(const SnlfLibraryDescriptor *);
  intptr_t (*bind)();
  bool (*unbind)(intptr_t);
};
typedef struct _SnlfModule SnlfModule;

static bool SnlfModuleUnload(SnlfModule *module) {
  if (module->binded) {
    if (module->unbind(module->context)) {
      SnlfWarningLogFormat("Unbind failed. Module path: %s", module->path);
    }
  }
  
  if (osutil_module_close(module->module)) {
    SnlfWarningLog("Unload failed");
    return true;
  }
  
  SnlfDealloc(module);
  return false;
}

static SnlfModule *SnlfModuleLoad(SnlfCoreRef core, const char *modulePath) {
  module_t native = osutil_module_open(modulePath);
  if (!native) {
    SnlfErrorLogFormat("Load failed. Module path: %s", modulePath);
    return NULL;
  }
  
  SnlfModule *module = SnlfAlloc(SnlfModule);
  if (!module) {
    SnlfOutOfMemoryError();
    return NULL;
  }
  module->core  = core;
  module->path  = modulePath;
  module->module = native;
  module->binded = false;
  module->context = NULL;
  
  module->getInfo = osutil_module_getsymbol(native, SNLF_MODULE_GET_INFO);
  if (!module->getInfo) {
    SnlfSymbolLoadFailedError(SNLF_MODULE_GET_INFO);
    SnlfModuleUnload(module);
    return NULL;
  }
  
  module->canBind = osutil_module_getsymbol(native, SNLF_MODULE_CAN_LOAD);
  if (!module->canBind) {
    SnlfSymbolLoadFailedError(SNLF_MODULE_CAN_BIND);
    SnlfModuleUnload(module);
    return NULL;
  }
  
  module->bind = osutil_module_getsymbol(native, SNLF_MODULE_LOAD);
  if (!module->bind) {
    SnlfSymbolLoadFailedError(SNLF_MODULE_BIND);
    SnlfModuleUnload(module);
    return NULL;
  }
  
  module->unbind = osutil_module_getsymbol(native, SNLF_MODULE_UNLOAD);
  if (!module->unbind) {
    SnlfSymbolLoadFailedError(SNLF_MODULE_UNBIND);
    SnlfModuleUnload(module);
    return NULL;
  }
  
  return module;
}

static bool SnlfModuleBind(SnlfModule *module, SnlfLibraryDescriptor *descriptor) {
  assert(!module->binded);
  
  if (!module->canBind(descriptor)) {
    SnlfWarningLogFormat("This module cannot bind. Module path: %s", module->path);
    return true;
  }
  
  intptr_t context = module->bind(module->core);
  if (!context) {
    SnlfWarningLogFormat("Bind failed. Module path: %s", module->path);
    return true;
  }
  
  module->binded = true;
  module->context = context;
  return false;
}

bool SnlfModuleLoadAll(SnlfCoreRef core) {
  SnlfLibraryDescriptor desc = { 0 };
  desc.compositorVersion = CPSR_API_VERSION;
  desc.compositorDriverType = CpsrGetDriverType();
  desc.sevenleafAPIVersion = SNLF_API_VERSION;
  desc.sevenleafLibraryVersion = SNLF_LIB_VERSION;

  const char *modulePathes[] = {
      "./Plugins/libgeneric/libgeneric.dylib",
      "./Plugins/libaplavcap/libaplavcap.dylib"
  };
  
  bool error = false;
  for (size_t i = 0; i < sizeof(modulePathes) / sizeof(modulePathes[0]); ++i) {
    const char *modulePath = modulePathes[i];
    SnlfModule *module = SnlfModuleLoad(core, modulePath);
    if (!module) {
      SnlfWarningLogFormat("Module load failed. Path: %s", modulePath);
      error = true;
      continue;
    }
    
    if (SnlfArrayAppend(core->modules, module)) {
      SnlfOutOfMemoryError();
      SnlfModuleUnload(module);
      error = true;
      continue;
    }
    
    if (SnlfModuleBind(module, &desc)) {
      error = true;
    }
  }
  return false;
}
