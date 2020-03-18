#include "SnlfCore+Private.h"

#include <assert.h>

#define LOCK(__CORE__)   pthread_mutex_lock(&__CORE__->inputMutex)
#define UNLOCK(__CORE__) pthread_mutex_unlock(&__CORE__->inputMutex)

SNLF_IMPLEMENTS_REFCOUNT(Input);

// ---
// Init/uninit
// ---
bool SnlfCoreInitForInputs(SnlfCoreRef core) {
  core->inputUniqueIdentifier = 0;
  SnlfArrayInit(core->inputs);
  SnlfArrayInit(core->inputHandlers);
  return SnlfRecursiveMutexCreate(&core->inputMutex);
}

bool SnlfCoreUninitForInputs(SnlfCoreRef core) {
  SnlfArrayRelease(core->inputs);
  SnlfArrayRelease(core->inputHandlers);
  return SnlfMutexDestroy(&core->inputMutex);
}

// ---
// Register
// ---
SnlfInputRef SnlfInputRegister(SnlfCoreRef core, SnlfInputDescriptor *descriptor) {
  assert(core);
  assert(descriptor);
  
  SnlfInputRef input = SnlfAllocRef(SnlfInput);
  if (!input) {
    SnlfOutOfMemoryError();
    return NULL;
  }
  
  // Set core & reference count
  input->core = core;
  osutil_atomic_store32(&input->refCount, 1);
  
  // Init
  osutil_atomic_store32(&input->activeCount, 0);
  input->identifier = ++core->inputUniqueIdentifier;
  input->descriptor = *descriptor;
  input->context = input->descriptor.init(descriptor);
  
  if (LOCK(core)) {
    SnlfMutexLockError();
    SnlfDealloc(input);
    return NULL;
  }
  
  if (SnlfArrayAppend(core->inputs, input)) {
    if (UNLOCK(core)) {
      SnlfMutexUnlockError();
    }
    SnlfDealloc(input);
    return NULL;
  }
  
  if (UNLOCK(core)) {
    SnlfMutexUnlockError();
  }
  return input;
}

// ---
// Activate/Deactivate
// ---
int32_t SnlfInputActivate(SnlfInputRef input) {
  int32_t ret = osutil_atomic_fetch_increment32(&input->activeCount);
#ifdef DEBUG_ACTIVE_COUNT
  SnlfVerboseLogFormat("Add active count: %d on %p (SnlfInput)", ret + 1, obj);
#endif
  if (ret == 0) {
    input->descriptor.activating(input->context, input->core->device);
  }
  return ++ret;
}

int32_t SnlfInputDeactivate(SnlfInputRef input) {
  int32_t ret = osutil_atomic_fetch_decrement32(&input->activeCount);
#ifdef DEBUG_ACTIVE_COUNT
  SnlfVerboseLogFormat("Release active count: %d on %p (SnlfInput)", ret - 1, obj);
#endif
  if (ret == 1) {
    input->descriptor.deactivated(input->context);
  }
  return --ret;
}

// ---
// Other
// ---
void SnlfEnumInputs(SnlfCoreRef core, SnlfInputProcedure enumFunc, intptr_t param) {
  if (LOCK(core)) {
    SnlfMutexLockError();
    return;
  }
  
  SnlfArrayForeach(core->inputs) {
    SnlfInputRef input = *(SnlfInputRef *)ptr;
    SnlfInputAddRef(input);
    enumFunc(input, param);
  }
  
  if (UNLOCK(core)) {
    SnlfMutexUnlockError();
  }
}

identifier_t SnlfInputGetIdentifier(SnlfInputRef input) {
  assert(input);
  
  return input->identifier;
}

const char *SnlfInputGetFriendlyName(SnlfInputRef input) {
  assert(input);
  
  return input->descriptor.friendlyName;
}
