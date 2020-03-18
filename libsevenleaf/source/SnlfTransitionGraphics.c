#include "SnlfGraphics+Private.h"

#include <assert.h>

#define LOCK(__CORE__)   pthread_mutex_lock(&__CORE__->transitionMutex)
#define UNLOCK(__CORE__) pthread_mutex_unlock(&__CORE__->transitionMutex)

// ---
// Transition Graphics
// ---
struct _SnlfTransitionGraphicsData {
  DEFINE_SNLF_GRAPHICS_COMMON_DATA;
  
  SnlfGraphicsData *previous;
  SnlfGraphicsData *current;
  
  timestamp_t transitionBegin;
  duration_t transitionDuration;
};

static inline void SnlfTransitionUninitPreviousData(SnlfTransitionGraphicsData *data) {
  SnlfGraphicsDataUninit(data->previous);
  data->previous = NULL;
}

static void SnlfTransitionGraphicsData_ProcessMessage(intptr_t _data, const SnlfGraphicsContext *context) {
  assert(_data);
  SnlfTransitionGraphicsData *data = (SnlfTransitionGraphicsData *)_data;
  
  SnlfBasicMessage *message;
  while ((message = (SnlfBasicMessage *)SnlfLockFreeQueueDequeue(&data->messageQueue))) {
    switch (message->type) {
    case SNLF_MESSAGE_TRANSITION_CHANGE_SCENE:
      if (data->previous) {
        SnlfTransitionUninitPreviousData(data);
      }
      if (data->current) {
        data->transitionBegin = 0;
        data->transitionDuration = 0;
        data->previous = data->current;
      }
      data->current = SnlfGraphicsDataInitFromSource((SnlfSourceRef)message->sender, context);
      break;
        
    default:
      break;
    }
    SnlfSourceRelease((SnlfSourceRef)message->sender);
    SnlfDealloc(message);
  }
  
  SnlfGraphicsData *previous = data->previous;
  if (previous) {
    SnlfGraphicsDataProcessMessage(previous, context);
  }
  
  SnlfGraphicsData *current = data->current;
  if (current) {
    SnlfGraphicsDataProcessMessage(current, context);
  }
}

static void SnlfTransitionGraphicsData_Update(intptr_t _data, SnlfGraphicsUpdateParams params) {
  assert(_data);
  SnlfTransitionGraphicsData *data = (SnlfTransitionGraphicsData *)_data;
  
  // Config transition state
  if (data->previous) {
    if (data->transitionBegin == 0) {
      data->transitionBegin = params.timestamp;
    }
    
    duration_t duration = params.timestamp - data->transitionBegin;
    if (data->transitionDuration <= duration) {
      SnlfTransitionUninitPreviousData(data);
    }
  }
  
  SnlfGraphicsData *previous = data->previous;
  if (previous) {
    SnlfGraphicsDataUpdate(previous, params);
  }
  
  if (data->current) {
    SnlfGraphicsDataUpdate(data->current, params);
  }
}

static void SnlfTransitionGraphicsData_Draw(intptr_t _data, SnlfGraphicsDrawParams params) {
  assert(_data);
  SnlfTransitionGraphicsData *data = (SnlfTransitionGraphicsData *)_data;
  
  SnlfGraphicsData *previous = data->previous;
  if (previous) {
    SnlfGraphicsDataDraw(previous, params);
  }
  
  SnlfGraphicsData *current = data->current;
  if (current) {
    SnlfGraphicsDataDraw(current, params);
  }
}

void SnlfTransitionGraphicsData_Uninit(intptr_t _data) {
  assert(_data);
  SnlfTransitionGraphicsData *data = (SnlfTransitionGraphicsData *)_data;
  SnlfDealloc(data);
}

SnlfTransitionGraphicsData *SnlfTransitionGraphicsDataInit(SnlfCoreRef core, const SnlfGraphicsContext *context) {
  SnlfTransitionGraphicsData *data = SnlfAlloc(SnlfTransitionGraphicsData);
  if (!context) {
    SnlfOutOfMemoryError();
    return NULL;
  }
  
  SnlfLockFreeQueueInit(&data->messageQueue);
  data->processMessage = SnlfTransitionGraphicsData_ProcessMessage;
  data->update = SnlfTransitionGraphicsData_Update;
  data->draw = SnlfTransitionGraphicsData_Draw;
  data->uninit = SnlfTransitionGraphicsData_Uninit;
  
  data->previous = NULL;
  data->current = NULL;
  return data;
}
