#import <QuartzCore/CAMetalLayer.h>

#define USE_SWAPCHAIN_PRIVATE_DEFINE 1
#include "CpsrGraphics+Private.h"

CpsrCommandBuffer *CpsrCommandBufferCreate(const CpsrCommandQueue *commandQueue) {
  CPSR_ASSUME(commandQueue);
  
  id<MTLCommandBuffer> native = [commandQueue->native commandBufferWithUnretainedReferences];
  if (!native) {
    return NULL;
  }
  
  CpsrCommandBuffer *commandBuffer = CpsrAlloc(CpsrCommandBuffer);
  if (commandBuffer) {
    commandBuffer->native = native;
    
#ifndef NDEBUG
    commandBuffer->commandQueue = commandQueue;
#endif
  }
  return commandBuffer;
}

void CpsrCommandBufferDestroy(CpsrCommandBuffer *commandBuffer) {
  CPSR_ASSUME(commandBuffer);
  
  [commandBuffer->native release];
  CpsrDealloc(commandBuffer);
}

void CpsrCommandBufferExecute(const CpsrCommandBuffer *commandBuffer) {
  CPSR_ASSUME(commandBuffer);
  
  if (commandBuffer->native.status == MTLCommandBufferStatusEnqueued) {
    [commandBuffer->native commit];
    [commandBuffer->native waitUntilCompleted];
  }
}

void CpsrCommandBufferExecuteAndPresent(const CpsrCommandBuffer *commandBuffer, const CpsrSwapChain *swapChain) {
  CPSR_ASSUME(commandBuffer);
  CPSR_ASSUME(swapChain);
  
  id<CAMetalDrawable> drawable = swapChain->currentDrawable;
  [commandBuffer->native presentDrawable:drawable];
  [commandBuffer->native addCompletedHandler:^(id buffer) {
    [drawable release];
  }];
  [commandBuffer->native commit];
}

void CpsrCommandBufferSingle(const CpsrCommandBuffer *commandBuffer, const CpsrFence *fence, uint64_t value) {
  CPSR_ASSUME(commandBuffer);
  CPSR_ASSUME(fence);
  
  [commandBuffer->native encodeSignalEvent:fence->native value:value];
}

void CpsrCommandBufferWait(const CpsrCommandBuffer *commandBuffer, const CpsrFence *fence, uint64_t value) {
  CPSR_ASSUME(commandBuffer);
  CPSR_ASSUME(fence);
  
  [commandBuffer->native encodeWaitForEvent:fence->native value:value];
}

#ifndef NDEBUG
void _CpsrCommandBufferPushDebugGroup(const CpsrCommandBuffer *commandBuffer, const char *groupName) {
  CPSR_ASSUME(commandBuffer);
  CPSR_ASSUME(groupName);
  
  NSString *nsGroupName = [[NSString alloc] initWithBytesNoCopy:(void *)groupName length:strlen(groupName) encoding:NSUTF8StringEncoding freeWhenDone:NO];
  [commandBuffer->native pushDebugGroup:nsGroupName];
  [nsGroupName release];
}

void _CpsrCommandBufferPopDebugGroup(const CpsrCommandBuffer *commandBuffer) {
  CPSR_ASSUME(commandBuffer);
  
  [commandBuffer->native popDebugGroup];
}
#endif
