#include "SnlfGraphics+Private.h"

#define LOCK(__CORE__)   pthread_mutex_lock(&__CORE__->sourceMutex)
#define UNLOCK(__CORE__) pthread_mutex_unlock(&__CORE__->sourceMutex)

// ---
// Source Graphics
// ---
typedef struct {
  DEFINE_SNLF_GRAPHICS_COMMON_DATA;
  
  SnlfSourceRef source;
  SNLF_ARRAY(SnlfGraphicsData *) children;
  
  bool enabled   : 1;
  bool animating : 1;
} SnlfSourceGraphicsData;

static void SnlfSourceAddSource(SnlfSourceGraphicsData *data, SnlfSourceRef source, const SnlfGraphicsContext *context) {
  assert(data);
  
  SnlfGraphicsData *childData = SnlfGraphicsDataInitFromSource(source, context);
  if (SnlfArrayAppend(data->children, childData)) {
    // TODO: Log
  }
}

static void SnlfSourceRemoveSource(SnlfSourceGraphicsData *data) {
  assert(data);
  
  data->uninit(data);
}

static void SnlfSourceGraphicsData_ProcessMessage(intptr_t _data, const SnlfGraphicsContext *context) {
  assert(_data);
  SnlfSourceGraphicsData *data = (SnlfSourceGraphicsData *)_data;
  
  SnlfBasicMessage *message;
  while ((message = (SnlfBasicMessage *)SnlfLockFreeQueueDequeue(&data->messageQueue))) {
    switch (message->type) {
    case SNLF_MESSAGE_SOURCE_ADD:
      SnlfSourceAddSource(data, (SnlfSourceRef)message->sender, context);
      break;
        
    case SNLF_MESSAGE_SOURCE_REMOVE:
      SnlfSourceRemoveSource(data);
      break;
        
    case SNLF_MESSAGE_SOURCE_ANIMATION:
      break;
        
    default:
        break;
    }
    SnlfDealloc(message);
  }
  
  SnlfArrayForeach(data->children) {
    SnlfGraphicsData *child = *(SnlfGraphicsData **)ptr;
    child->processMessage(child, context);
  }
}

static void SnlfSourceGraphicsData_Update(intptr_t _data, SnlfGraphicsUpdateParams params) {
  assert(_data);
  
  SnlfSourceGraphicsData *data = (SnlfSourceGraphicsData *)_data;
  if (data->enabled) {
    params.world = matrix4x4_mul(params.world, data->source->transform);
    
    SnlfArrayForeach(data->children) {
      SnlfGraphicsData *child = *(SnlfGraphicsData **)ptr;
      child->update(child, params);
    }
  }
}

static void SnlfSourceGraphicsData_Draw(intptr_t _data, SnlfGraphicsDrawParams params) {
  assert(_data);
  
  SnlfSourceGraphicsData *data = (SnlfSourceGraphicsData *)_data;
  if (data->enabled) {
    SnlfArrayForeach(data->children) {
      SnlfGraphicsData *childData = *(SnlfGraphicsData **)ptr;
      childData->draw(childData, params);
    }
  }
}

void SnlfSourceGraphicsData_Uninit(intptr_t _data) {
  assert(_data);
  
  SnlfSourceGraphicsData *data = (SnlfSourceGraphicsData *)_data;
  SnlfArrayForeach(data->children) {
    SnlfGraphicsData *child = *(SnlfGraphicsData **)ptr;
    SnlfGraphicsDataUninit(child);
  }
  SnlfSourceRelease(data->source);
  SnlfDealloc(data);
}

static inline void SnlfGraphicsDataInitChildSources(SnlfSourceGraphicsData *parentData, const SnlfGraphicsContext *context) {
  SnlfSourceRef parent = parentData->source;
  assert(parent);
  
  SnlfCoreRef core = parent->core;
  assert(core);
  
  if (LOCK(core)) {
    SnlfMutexLockError();
    return;
  }
  
  SnlfArrayForeach(parent->children) {
    SnlfSourceRef child = *(SnlfSourceRef *)ptr;
    SnlfGraphicsData *childData = SnlfGraphicsDataInitFromSource(child, context);
    if (!childData) {
      continue;
    }
    
    if (SnlfArrayAppend(parentData->children, childData)) {
      SnlfGraphicsDataUninit(childData);
    }
  }
  
  if (UNLOCK(core)) {
    SnlfMutexUnlockError();
  }
}


extern SnlfGraphicsData *SnlfGraphicsDataInitForInput(SnlfSourceRef source, const SnlfGraphicsContext *context);
extern SnlfGraphicsData *SnlfGraphicsDataInitForGraphicsGenerator(SnlfSourceRef source, const SnlfGraphicsContext *context);

SnlfGraphicsData *SnlfGraphicsDataInitFromSource(SnlfSourceRef source, const SnlfGraphicsContext *context) {
  SnlfSourceAddRef(source);
  
  switch (source->type) {
  case SNLF_SOURCE_INPUT:
    return SnlfGraphicsDataInitForInput(source, context);
      
  case SNLF_SOURCE_GRAPHICS_GENERATOR:
    return SnlfGraphicsDataInitForGraphicsGenerator(source, context);
      
  case SNLF_SOURCE_REFERENCE:
  default:
    break;
  }
  
  SnlfSourceGraphicsData *data = SnlfAlloc(SnlfSourceGraphicsData);
  if (!data) {
    SnlfOutOfMemoryError();
    SnlfSourceRelease(source);
    return NULL;
  }
  
  if (SnlfArrayAppend(source->graphicsData, data)) {
    SnlfOutOfMemoryError();
    SnlfDealloc(data);
    SnlfSourceRelease(source);
    return NULL;
  }
  
  SnlfLockFreeQueueInit(&data->messageQueue);
  data->processMessage = SnlfSourceGraphicsData_ProcessMessage;
  data->update = SnlfSourceGraphicsData_Update;
  data->draw = SnlfSourceGraphicsData_Draw;
  data->uninit = SnlfSourceGraphicsData_Uninit;
  
  data->source = source;
  SnlfArrayInit(data->children);
  data->enabled = true;
  data->animating = false;

  SnlfGraphicsDataInitChildSources(data, context);
  return (SnlfGraphicsData *)data;
}
