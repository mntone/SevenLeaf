#include "CpsrGraphics+Private.h"

CpsrCommandQueue *CpsrCommandQueueCreate(const CpsrDevice *device) {
  CPSR_ASSUME(device);

  id<MTLCommandQueue> native = [device->native newCommandQueue];
  if (!native) {
    return NULL;
  }
  
  CpsrCommandQueue *commandQueue = CpsrAlloc(CpsrCommandQueue);
  if (commandQueue) {
    commandQueue->native = native;
    
#ifndef NDEBUG
    commandQueue->device = device;
#endif
  }
  return commandQueue;
}

void CpsrCommandQueueDestroy(CpsrCommandQueue *commandQueue) {
  CPSR_ASSUME(commandQueue);

  [commandQueue->native release];
  CpsrDealloc(commandQueue);
}
