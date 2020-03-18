#include "SnlfCore+Private.h"

#define LOCK(__CORE__)   pthread_mutex_lock(&__CORE__->transitionMutex)
#define UNLOCK(__CORE__) pthread_mutex_unlock(&__CORE__->transitionMutex)

// ---
// Init/Uninit
// ---
bool SnlfCoreInitForTransition(SnlfCoreRef core) {
  core->currentSource = NULL;
  SnlfArrayInit(core->transitionHandlers);
  return SnlfMutexCreate(&core->transitionMutex);
}

bool SnlfCoreUninitForTransition(SnlfCoreRef core) {
  SnlfArrayRelease(core->transitionHandlers);
  return SnlfMutexDestroy(&core->transitionMutex);
}

// ---
// Notification
// ---
static inline bool SnlfTransitionDispatchChanged(SnlfCoreRef core, SnlfSourceRef previous, SnlfSourceRef current) {
  SnlfSourceChangedArgs args;
  args.previousSource = previous;
  args.currentSource = current;
  SnlfArrayForeach(core->transitionHandlers) {
    SnlfSourceChangedBag *bag = *(SnlfSourceChangedBag **)ptr;
    bag->handler(core, args, bag->param);
  }
  return false;
}

// ---
// Transition
// ---
bool SnlfTransitionDispatch(SnlfCoreRef core, SnlfSourceRef source) {
  if (LOCK(core)) {
    return true;
  }
  
  SnlfSourceRef previousSource = core->currentSource;
  core->currentSource = source;
  
  // Dispatch to internal
  SnlfBasicMessage *message = SnlfMessageCreateFromSource(SNLF_MESSAGE_TRANSITION_CHANGE_SCENE, source);
  if (!message) {
    return true;
  }
  SnlfMessageDispatchToRoot(core, message);
  
  // Dispatch to callback
  SnlfTransitionDispatchChanged(core, previousSource, source);
  
  if (UNLOCK(core)) {
    // TODO: Log
  }
  return false;
}

// ---
// Transision change handler
// ---
intptr_t SnlfTransitionAddNotificationHandler(SnlfCoreRef core, SnlfSourceChangedHandler handler, intptr_t param) {
  SnlfSourceChangedBag *bag = SnlfAlloc(SnlfSourceChangedBag);
  if (!bag) {
    SnlfOutOfMemoryError();
    return NULL;
  }
  
  if (LOCK(core)) {
    SnlfDealloc(bag);
    return NULL;
  }
  
  if (SnlfArrayAppend(core->transitionHandlers, bag)) {
    SnlfDealloc(bag);
    return NULL;
  }
  
  if (UNLOCK(core)) {
    // TODO: Log
  }
  
  bag->handler = handler;
  bag->param = param;
  return (intptr_t)bag;
}

bool SnlfTransitionRemoveNotificationHandler(SnlfCoreRef core, intptr_t handle) {
  SnlfSourceChangedBag *bag = (SnlfSourceChangedBag *)handle;
  if (LOCK(core)) {
    return true;
  }
  
  SnlfArraySizeType index;
  if (SnlfArrayRemove(core->transitionHandlers, bag, &index)) {
    // TODO: Log
  }
  
  if (UNLOCK(core)) {
    // TODO: Log
  }
  
  SnlfDealloc(bag);
  return false;
}

