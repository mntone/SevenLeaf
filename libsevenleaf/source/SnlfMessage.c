#include "SnlfGraphics+Private.h"
#include "SnlfMessage.h"

SnlfBasicMessage *SnlfMessageCreateFromSource(SnlfMessageType type, SnlfSourceRef source) {
  SnlfSourceAddRef(source);
  
  SnlfBasicMessage *message = SnlfAlloc(SnlfBasicMessage);
  if (!message) {
    SnlfSourceRelease(source);
    SnlfOutOfMemoryError();
    return NULL;
  }
  
  message->type = SNLF_MESSAGE_TRANSITION_CHANGE_SCENE;
  message->timestamp = osutil_gettime_as_nanoseconds();
  message->sender = source;
  return message;
}

void SnlfMessageDispatchToRoot(SnlfCoreRef core, const SnlfBasicMessage *message) {
  SnlfGraphicsThreadContext *graphicsThreadContext = core->graphicsThreadContext;
  if (graphicsThreadContext) {
    SnlfLockFreeQueueEnqueue(&graphicsThreadContext->root->messageQueue, message);
  }
}

void SnlfMessageDispatchToSource(SnlfSourceRef source, const SnlfBasicMessage *message) {
  SnlfSourceAddRef(source);
  SnlfArrayForeach(source->graphicsData) {
    SnlfGraphicsData *data = *(SnlfGraphicsData **)ptr;
    SnlfLockFreeQueueEnqueue(&data->messageQueue, message);
  }
  SnlfSourceRelease(source);
}
