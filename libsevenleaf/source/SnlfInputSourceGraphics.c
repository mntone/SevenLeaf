#include "SnlfGraphics+Private.h"

// ---
// Input Source Graphics
// ---
typedef struct {
  DEFINE_SNLF_GRAPHICS_COMMON_DATA;
  
  SnlfSourceRef source;
  
  bool enabled   : 1;
  bool animating : 1;
} SnlfInputSourceGraphicsData;

static void SnlfInputSourceGraphicsDataProcessMessage(intptr_t _data, const SnlfGraphicsContext *context) {
  SnlfAssume(_data);
  SnlfInputSourceGraphicsData *data = (SnlfInputSourceGraphicsData *)_data;
  
  const SnlfBasicMessage *message;
  while ((message = (const SnlfBasicMessage *)SnlfLockFreeQueueDequeue(&data->messageQueue))) {
    switch (message->type) {
    case SNLF_MESSAGE_SOURCE_ANIMATION:
      break;
        
    default:
        break;
    }
    SnlfDealloc(message);
  }
}

static void SnlfInputSourceGraphicsDataUpdate(intptr_t _data, SnlfGraphicsUpdateParams params) {
  SnlfAssume(_data);
  
  SnlfInputSourceGraphicsData *data = (SnlfInputSourceGraphicsData *)_data;
  if (data->enabled) {
    SnlfInputRef input = data->source->input;
    SnlfAssume(input);
    
    //input->descriptor.update(input->context, params);
  }
}

static void SnlfInputSourceGraphicsDataDraw(intptr_t _data, SnlfGraphicsDrawParams params) {
  SnlfAssume(_data);
  
  SnlfInputSourceGraphicsData *data = (SnlfInputSourceGraphicsData *)_data;
  if (data->enabled) {
    SnlfInputRef input = data->source->input;
    SnlfAssume(input);
    
    //input->descriptor.draw(input->context, params);
  }
}

void SnlfInputSourceGraphicsDataUninit(intptr_t _data) {
  SnlfAssume(_data);
  
  SnlfInputSourceGraphicsData *data = (SnlfInputSourceGraphicsData *)_data;
  SnlfSourceRelease(data->source);
  SnlfDealloc(data);
}

SnlfGraphicsData *SnlfGraphicsDataInitForInput(SnlfSourceRef source, const SnlfGraphicsContext *context) {
  SnlfInputSourceGraphicsData *data = SnlfAlloc(SnlfInputSourceGraphicsData);
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
  data->processMessage = SnlfInputSourceGraphicsDataProcessMessage;
  data->update = SnlfInputSourceGraphicsDataUpdate;
  data->draw = SnlfInputSourceGraphicsDataDraw;
  data->uninit = SnlfInputSourceGraphicsDataUninit;
  
  data->source = source;
  data->enabled = true;
  data->animating = false;
  return (SnlfGraphicsData *)data;
}
