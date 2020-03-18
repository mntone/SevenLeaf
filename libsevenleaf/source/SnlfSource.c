#include "SnlfCore+Private.h"

#define LOCK(__CORE__)   pthread_mutex_lock(&__CORE__->sourceMutex)
#define UNLOCK(__CORE__) pthread_mutex_unlock(&__CORE__->sourceMutex)

// ---
// Init/Uninit
// ---
bool SnlfCoreInitForSources(SnlfCoreRef core) {
  SnlfArrayInit(core->sources);
  SnlfArrayInit(core->sourceHandlers);
  return SnlfRecursiveMutexCreate(&core->sourceMutex);
}

bool SnlfCoreUninitForSources(SnlfCoreRef core) {
  SnlfArrayRelease(core->sources);
  SnlfArrayRelease(core->sourceHandlers);
  return SnlfMutexDestroy(&core->sourceMutex);
}

// ---
// Create/Destory/AddRef/Release
// ---
extern inline SnlfSourceRef SnlfSourceCreateDefault(SnlfCoreRef core) {
  SnlfSourceRef source = SnlfAllocRef(SnlfSource);
  if (!source) {
    SnlfOutOfMemoryError();
    return NULL;
  }
  
  // Set core & reference count
  source->core = core;
  osutil_atomic_store32(&source->refCount, 1);
  
  // Init info
  SnlfArrayInit(source->handlers);
  
  // Init flags
  source->enabled     = true;
  source->interaction = true;
  
  // Init relations
  source->input  = NULL;
  source->parent = NULL;
  SnlfArrayInit(source->children);
  
  // Init graphics resources
  source->transform = matrix4x4_idt();
  SnlfArrayInit(source->graphicsData);
  SnlfArrayInit(source->backdropTransformers);
  SnlfArrayInit(source->userTransformers);
  
  // Init interactions
  source->click        = NULL;
  source->pointerDown  = NULL;
  source->pointerUp    = NULL;
  source->pointerEnter = NULL;
  source->pointerLeave = NULL;
  source->wheelMove    = NULL;
  return source;
}

SnlfSourceRef SnlfSourceCreate(SnlfCoreRef core) {
  assert(core);
  
  SnlfSourceRef source = SnlfSourceCreateDefault(core);
  if (!source) {
    return NULL;
  }
  
  source->type = SNLF_SOURCE_GROUP;
  return source;
}

SnlfSourceRef SnlfSourceCreateFromInput(SnlfInputRef input) {
  assert(input);
  SnlfInputAddRef(input);
  
  SnlfSourceRef source = SnlfSourceCreateDefault(input->core);
  if (!source) {
    SnlfInputRelease(input);
    return NULL;
  }
  
  input->descriptor.activating(input->context, input->core->device);
  
  source->type = SNLF_SOURCE_INPUT;
  source->input = input;
  return source;
}

SnlfSourceRef SnlfSourceCreateFromGraphicsGenerator(SnlfCoreRef core, SnlfGraphicsGeneratorRef generator) {
  assert(generator);
  
  SnlfSourceRef source = SnlfSourceCreateDefault(core);
  if (!source) {
    return NULL;
  }
  
  source->type = SNLF_SOURCE_GRAPHICS_GENERATOR;
  source->graphicsGenerator = generator;
  return source;
}

extern inline bool SnlfSourceDestroy(SnlfSourceRef source) {
  // Remove children
  for (size_t i = 0; i < source->children.size; ++i) {
    SnlfSourceRef child = source->children.data[i];
    SnlfSourceRemoveFromParent(child);
    SnlfSourceRelease(source);
  }
  
  SnlfDealloc(source);
  return false;
}

SNLF_IMPLEMENTS_REFCOUNT_CUSTOM(Source)

// ---
// Property
// ---
matrix4x4_t SnlfSourceGetTransform(SnlfSourceRef source) {
  assert(source);
  return source->transform;
}

void SnlfSourceSetTransform(SnlfSourceRef source, matrix4x4_t transform) {
  assert(source);
  source->transform = transform;
}

uint32_t SnlfSourceGetPropertyCount(SnlfSourceRef source) {
  assert(source);
  assert(source->type == SNLF_SOURCE_GRAPHICS_GENERATOR);
  return source->graphicsGenerator->getPropertyCount();
}

bool SnlfSourceGetPropertyKeys(SnlfSourceRef source, identifier_t identifiers[], uint32_t count) {
  assert(source);
  assert(source->type == SNLF_SOURCE_GRAPHICS_GENERATOR);
  return source->graphicsGenerator->getPropertyKeys(identifiers, count);
}

SnlfBox SnlfSourceGetProperty(SnlfSourceRef source, identifier_t identifier) {
  
}

bool SnlfSourceSetProperty(SnlfSourceRef source, identifier_t identifier, SnlfBox value) {
  
}

// ---
// Tree operation
// ---
void SnlfSourceEnumChildSources(SnlfSourceRef source, SnlfSourceProcedure enumFunc, intptr_t param) {
  SnlfArrayForeach(source->children) {
    SnlfSourceRef source = *(SnlfSourceRef *)ptr;
    SnlfSourceAddRef(source);
    enumFunc(source, param);
  }
}

SnlfSourceRef SnlfRootSourceGetCurrent(SnlfCoreRef core) {
  if (LOCK(core)) {
    SnlfMutexLockError();
    return NULL;
  }
  
  SnlfSourceRef source = core->currentSource;
  if (UNLOCK(core)) {
    SnlfMutexUnlockError();
  }
  return source;
}

SnlfSourceRef SnlfRootSourceGetAt(SnlfCoreRef core, uint32_t index) {
  if (LOCK(core)) {
    SnlfMutexLockError();
    return NULL;
  }
  
  SnlfSourceRef source = SnlfSourceArrayGetAt(core->sources, index);
  if (UNLOCK(core)) {
    SnlfMutexUnlockError();
  }
  return source;
}

static inline bool SnlfRootSourceArrayRaiseChange(SnlfCoreRef core, SnlfArrayChangedArgs args) {
  SnlfArrayForeach(core->sourceHandlers) {
    SnlfArrayChangedBag *bag = *(SnlfArrayChangedBag **)ptr;
    bag->handler(core, args, bag->param);
  }
  
  if (UNLOCK(core)) {
    SnlfMutexUnlockError();
  }
  return false;
}

bool SnlfRootSourceAppend(SnlfSourceRef source) {
  assert(source);
  SnlfSourceAddRef(source);
  
  SnlfCoreRef core = source->core;
  if (LOCK(core)) {
    SnlfMutexLockError();
    SnlfSourceRelease(source);
    return true;
  }
  
  if (SnlfArrayAppend(core->sources, source)) {
    SnlfSourceRelease(source);
    if (UNLOCK(core)) {
      SnlfMutexUnlockError();
    }
    return true;
  }
  
  if (SnlfTransitionDispatch(core, source)) {
    SnlfSourceRelease(source);
    if (UNLOCK(core)) {
      SnlfMutexUnlockError();
    }
    // TODO: remove from array
    return true;
  }
  
  SnlfArrayChangedArgs args;
  args.operation = SNLF_OPERATION_ADD;
  args.oldIndex = 0;
  args.newIndex = core->sources.size - 1;
  args.length = 1;
  return SnlfRootSourceArrayRaiseChange(core, args);
}

static inline bool SnlfSourceArrayRaiseChange(SnlfCoreRef core, SnlfSourceRef source, SnlfArrayChangedArgs args) {
  SnlfArrayForeach(source->handlers) {
    SnlfArrayChangedBag *bag = *(SnlfArrayChangedBag **)ptr;
    bag->handler(source, args, bag->param);
  }
  
  if (UNLOCK(core)) {
    SnlfMutexUnlockError();
  }
  return false;
}

bool SnlfSourceAppend(SnlfSourceRef child, SnlfSourceRef parent) {
  assert(parent);
  assert(child);
  assert(!child->parent);
  SnlfSourceAddRef(parent);
  SnlfSourceAddRef(child);
  
  SnlfCoreRef core = child->core;
  if (LOCK(core)) {
    SnlfMutexLockError();
    SnlfSourceRelease(parent);
    SnlfSourceRelease(child);
    return true;
  }
  
  if (SnlfArrayAppend(parent->children, child)) {
    SnlfSourceRelease(parent);
    SnlfSourceRelease(child);
    if (UNLOCK(core)) {
      SnlfMutexUnlockError();
    }
    return true;
  }
  
  // Set params
  child->parent = parent;
  
  // Dispatch to internal
  SnlfBasicMessage *message = SnlfMessageCreateFromSource(SNLF_MESSAGE_SOURCE_ADD, child);
  if (!message) {
    return true;
  }
  SnlfMessageDispatchToSource(parent, message);

  // Dispatch to callback
  SnlfArrayChangedArgs args;
  args.operation = SNLF_OPERATION_ADD;
  args.oldIndex = index;
  args.newIndex = 0;
  args.length = 1;
  
  bool ret = SnlfSourceArrayRaiseChange(core, child, args);
  SnlfSourceRelease(parent);
  return ret;
}

bool SnlfSourceInsertAt(SnlfArraySizeType index, SnlfSourceRef source, SnlfSourceRef parent) {
  assert(source);
  SnlfSourceAddRef(source);
  
  if (SnlfArrayInsertAt(parent->children, index, source)) {
    return true;
  }

  // TODO: Dispatch message
  return false;
}

bool SnlfSourceInsertBelow(SnlfSourceRef source, SnlfSourceRef sibling) {
  
}

bool SnlfSourceInsertAbove(SnlfSourceRef source, SnlfSourceRef sibling) {
  
}

bool SnlfSourceRemoveFromParent(SnlfSourceRef source) {
  SnlfCoreRef core = source->core;
  assert(core);
  
  // Remove source from parent source.
  SnlfSourceRef parent = source->parent;
  if (parent) {
    SnlfArraySizeType index;
    if (SnlfArrayRemove(parent->children, source, &index)) {
      // TODO: exit
      return true;
    }
    
    SnlfArrayChangedArgs args;
    args.operation = SNLF_OPERATION_REMOVE;
    args.oldIndex = index;
    args.newIndex = 0;
    args.length = 1;
    
    bool ret = SnlfSourceArrayRaiseChange(core, source, args);
    SnlfSourceRelease(source);
    return ret;
  }
  
  // Remove source from root.
  if (core) {
    SnlfArraySizeType index;
    if (SnlfArrayRemove(core->sources, source, &index)) {
      // TODO: exit
      return true;
    }
    
    SnlfArrayChangedArgs args;
    args.operation = SNLF_OPERATION_REMOVE;
    args.oldIndex = index;
    args.newIndex = 0;
    args.length = 1;
    
    bool ret = SnlfRootSourceArrayRaiseChange(core, args);
    SnlfSourceRelease(source);
    return ret;
  }
  
  return true;
}

// ---
// Array change handler
// ---
intptr_t SnlfCoreRootSourceAddNotificationHandler(SnlfCoreRef core, SnlfArrayChangedHandler handler, intptr_t param) {
  SnlfArrayChangedBag *bag = SnlfAlloc(SnlfArrayChangedBag);
  if (!bag) {
    SnlfOutOfMemoryError();
    return NULL;
  }
  
  if (LOCK(core)) {
    SnlfMutexLockError();
    SnlfDealloc(bag);
    return NULL;
  }
  
  if (SnlfArrayAppend(core->sourceHandlers, bag)) {
    SnlfDealloc(bag);
    return NULL;
  }
  
  if (UNLOCK(core)) {
    SnlfMutexUnlockError();
  }
  
  bag->array = &core->sourceHandlers.__bag;
  bag->itemSize = sizeof(*core->sourceHandlers.data);
  bag->handler = handler;
  bag->param = param;
  return bag;
}

bool SnlfCoreRootSourceRemoveNotificationHandler(SnlfCoreRef core, intptr_t handle) {
  SnlfArrayChangedBag *bag = (SnlfArrayChangedBag *)handle;
  if (LOCK(core)) {
    SnlfMutexLockError();
    return true;
  }
  
  SnlfArraySizeType index;
  if (_SnlfArrayRemove(bag->array, bag, &index, bag->itemSize)) {
    // TODO: Log
  }
  
  if (UNLOCK(core)) {
    SnlfMutexUnlockError();
  }
  
  SnlfDealloc(bag);
  return false;
}

// ---
// Event implementation
// ---
static inline void SnlfSourceProcessClick(SnlfSourceRef source, bool *handled) {
  if (!source->interaction) {
    return;
  }
  
  // TODO: ローカル座標に変換。[0, 1] (クロップなし) の範囲に入ってるか確認。入っていなければ return
  if (true) {
    return;
  }
  
  source->click(handled);
  if (*handled) {
    return;
  }
  
  switch (source->type) {
  case SNLF_SOURCE_INPUT:
    break;
      
  case SNLF_SOURCE_GRAPHICS_GENERATOR:
    break;
      
  case SNLF_SOURCE_REFERENCE: {
    SnlfSourceRef reference = source->reference;
    SnlfAssume(reference);
    SnlfSourceProcessClick(reference, handled);
    break;
  }
      
  default: {
    SnlfArrayForeach(source->children) {
      SnlfSourceRef child = *(SnlfSourceRef *)ptr;
      SnlfSourceProcessClick(child, handled);
      if (*handled) {
        break;
      }
    }
    break;
  }
  }
}

void SnlfSourceRaiseClick(SnlfCoreRef core) {
  if (LOCK(core)) {
    SnlfMutexLockError();
    return;
  }
  
  bool handled = false;
  for (size_t i = 0; i < core->sources.size; ++i) {
    SnlfSourceRef child = core->sources.data[i];
    SnlfSourceProcessClick(child, &handled);
    if (handled) {
      break;
    }
  }
  
  if (UNLOCK(core)) {
    SnlfMutexUnlockError();
  }
}
