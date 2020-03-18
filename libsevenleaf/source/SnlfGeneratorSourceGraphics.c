#include "SnlfGraphics+Private.h"

// ---
// Generator Source Graphics
// ---
typedef struct {
  DEFINE_SNLF_GRAPHICS_COMMON_DATA;
  
  SnlfSourceRef source;
  intptr_t context;
  
  bool enabled   : 1;
  bool animating : 1;
} SnlfGeneratorSourceGraphicsData;

static void SnlfGeneratorSourceGraphicsData_ProcessMessage(intptr_t _data, const SnlfGraphicsContext *context) {
  SnlfAssume(_data);
  SnlfGeneratorSourceGraphicsData *data = (SnlfGeneratorSourceGraphicsData *)_data;
  
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

static void SnlfGeneratorSourceGraphicsData_Update(intptr_t _data, SnlfGraphicsUpdateParams params) {
  SnlfAssume(_data);
  SnlfGeneratorSourceGraphicsData *data = (SnlfGeneratorSourceGraphicsData *)_data;
  if (data->enabled) {
    params.world = matrix4x4_mul(params.world, data->source->transform);
    
    SnlfGraphicsGeneratorRef generator = data->source->graphicsGenerator;
    SnlfAssume(generator);
    
    generator->update(data->context, params);
  }
}

static void SnlfGeneratorSourceGraphicsData_Draw(intptr_t _data, SnlfGraphicsDrawParams params) {
  SnlfAssume(_data);
  SnlfGeneratorSourceGraphicsData *data = (SnlfGeneratorSourceGraphicsData *)_data;
  if (data->enabled) {
    SnlfGraphicsGeneratorRef generator = data->source->graphicsGenerator;
    SnlfAssume(generator);
    
    generator->draw(data->context, params);
  }
}

void SnlfGeneratorSourceGraphicsData_Uninit(intptr_t _data) {
  SnlfAssume(_data);
  SnlfGeneratorSourceGraphicsData *data = (SnlfGeneratorSourceGraphicsData *)_data;
  data->source->graphicsGenerator->uninit(data->context);
  SnlfDealloc(data);
}

SnlfGraphicsData *SnlfGraphicsDataInitForGraphicsGenerator(SnlfSourceRef source, const SnlfGraphicsContext *context) {
  SnlfGeneratorSourceGraphicsData *data = SnlfAlloc(SnlfGeneratorSourceGraphicsData);
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
  data->processMessage = SnlfGeneratorSourceGraphicsData_ProcessMessage;
  data->update = SnlfGeneratorSourceGraphicsData_Update;
  data->draw = SnlfGeneratorSourceGraphicsData_Draw;
  data->uninit = SnlfGeneratorSourceGraphicsData_Uninit;
  
  data->source = source;
  data->context = data->source->graphicsGenerator->init(context);
  
  data->enabled = true;
  data->animating = false;
  return (SnlfGraphicsData *)data;
}
