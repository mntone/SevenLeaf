#include "CpsrGraphics+Private.h"

static inline CpsrFence *CpsrFenceCreateFromNative(const CpsrDevice *device, CpsrFenceType fenceType, id<MTLEvent> native) {
  if (!native) {
    return NULL;
  }
  
  CpsrFence *fence = CpsrAlloc(CpsrFence);
  if (fence) {
    fence->native = native;
    
#ifndef NDEBUG
    fence->device = device;
    fence->fenceType = fenceType;
#endif
  }
  return fence;
}

CpsrFence *CpsrFenceCreate(const CpsrDevice *device, CpsrFenceType fenceType) {
  CPSR_ASSUME(device);
  
  if (fenceType == CPSR_FENCE_SHARED) {
    id<MTLSharedEvent> native = [device->native newSharedEvent];
    return CpsrFenceCreateFromNative(device, fenceType, native);
  } else {
    id<MTLEvent> native = [device->native newEvent];
    return CpsrFenceCreateFromNative(device, fenceType, native);
  }
}

void CpsrFenceDestroy(CpsrFence *fence) {
  CPSR_ASSUME(fence);
  assert(fence->native.retainCount == 1);
  
  [fence->native release];
  CpsrDealloc(fence);
}
