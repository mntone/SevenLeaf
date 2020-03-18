#include "SnlfCore+Private.h"

// ---
// Init/uninit
// ---
bool SnlfCoreInitForGenerators(SnlfCoreRef core) {
  SnlfArrayInit(core->graphicsGenerators);
  return false;
}

bool SnlfCoreUninitForGenerators(SnlfCoreRef core) {
  SnlfArrayRelease(core->graphicsGenerators);
  return false;
}

// ---
// Register/Unregister
// ---
bool SnlfGraphicsGeneratorRegister(SnlfCoreRef core, SnlfGraphicsGeneratorRef generator) {
  assert(core);
  assert(generator);
  
  return SnlfArrayAppend(core->graphicsGenerators, generator);
}

void SnlfGraphicsGeneratorUnregister(SnlfCoreRef core, SnlfGraphicsGeneratorRef generator) {
  assert(core);
  assert(generator);
  
  SnlfArraySizeType index;
  SnlfArrayRemove(core->graphicsGenerators, generator, &index);
}

void SnlfEnumGraphicsGenerators(SnlfCoreRef core, SnlfGraphicsGeneratorProcedure enumFunc, intptr_t param) {
  SnlfArrayForeach(core->graphicsGenerators) {
    SnlfGraphicsGeneratorRef generator = *(SnlfGraphicsGeneratorRef *)ptr;
    enumFunc(generator, param);
  }
}
