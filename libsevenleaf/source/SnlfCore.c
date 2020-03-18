#include "SnlfCore+Private.h"
#include <assert.h>

SnlfCoreRef SnlfCoreInit(SnlfCoreStartupArgs args) {
  SnlfCoreRef core = SnlfAllocRef(SnlfCore);
  if (!core) {
    SnlfOutOfMemoryError();
    return NULL;
  }
  
  // Init modules
  SnlfArrayInit(core->modules);
  
  // Init
  if (SnlfCoreInitForGenerators(core)
      || SnlfCoreInitForInputs(core)
      || SnlfCoreInitForTransition(core)
      || SnlfCoreInitForSources(core)
      || SnlfCoreInitForDisplays(core)) {
    return NULL;
  }
  
  // Start graphics service
  if (SnlfGraphicsInit(core)) {
    return NULL;
  }
  
  // Start sound service
  
  return core;
}

// Add service entry point
// - Hotkey service
// - Plugin service

void SnlfCoreUninit(SnlfCoreRef core) {
  assert(core);
  
  SnlfGraphicsUninit(core);
  SnlfCoreUninitForDisplays(core);
  SnlfCoreUninitForSources(core);
  SnlfCoreUninitForTransition(core);
  SnlfCoreUninitForInputs(core);
  SnlfCoreUninitForGenerators(core);
  SnlfDealloc(core);
}
