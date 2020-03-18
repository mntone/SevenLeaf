#include "CpsrGraphics+Private.h"

CpsrComputeContext *CpsrComputeContextCreate(const CpsrCommandBuffer *commandBuffer) {
  CPSR_ASSUME(commandBuffer);

  CpsrComputeContext *computeContext = CpsrAlloc(CpsrComputeContext);
  if (computeContext) {
    computeContext->commandBuffer = commandBuffer;
    computeContext->native = NULL;
  }
  return computeContext;
}

void CpsrComputeContextDestroy(CpsrComputeContext *computeContext) {
  CPSR_ASSUME(computeContext);

  if (computeContext->native) {
    assert(computeContext->native.retainCount == 1);
    [computeContext->native release];
  }
  CpsrDealloc(computeContext);
}

bool CpsrComputeContextSetPipelineState(CpsrComputeContext *computeContext, CpsrComputePipelineState *pipelineState) {
  assert(computeContext);
  assert(!computeContext->native);
  assert(pipelineState);

  id<MTLComputePipelineState> nativePipelineState = CpsrComputePipelineStateGetNative(pipelineState);
  if (!nativePipelineState) {
    // TODO: log
    return true;
  }

  id<MTLComputeCommandEncoder> commandEncoder = [computeContext->commandBuffer->native computeCommandEncoder];
  [commandEncoder setComputePipelineState:nativePipelineState];
  computeContext->native = commandEncoder;
  return false;
}

void CpsrComputeContextSetConstantBuffer(CpsrComputeContext *computeContext,
                                         uint8_t index,
                                         const CpsrBuffer *vertexBuffer) {
  CPSR_ASSUME(computeContext);
  CPSR_ASSUME(computeContext->native);
  CPSR_ASSUME(vertexBuffer);
  CPSR_ASSUME(vertexBuffer->bufferType == CPSR_CONSTANT_BUFFER);

  [computeContext->native setBuffer:vertexBuffer->native offset:0 atIndex:index];
}

void CpsrComputeContextSetReadTexture(CpsrComputeContext *computeContext, uint8_t index, const CpsrTexture2D *texture) {
  CPSR_ASSUME(computeContext);
  CPSR_ASSUME(computeContext->native);
  CPSR_ASSUME(index < 16);
  CPSR_ASSUME(texture);
  CPSR_ASSUME(texture->usage & CPSR_TEXTURE_USAGE_READ);

  [computeContext->native setTexture:texture->native atIndex:index];
}

void CpsrComputeContextSetWriteTexture(CpsrComputeContext *computeContext,
                                       uint8_t index,
                                       const CpsrTexture2D *texture) {
  CPSR_ASSUME(computeContext);
  CPSR_ASSUME(computeContext->native);
  CPSR_ASSUME(index < 16);
  CPSR_ASSUME(texture);
  CPSR_ASSUME(texture->usage & CPSR_TEXTURE_USAGE_WRITE);
  
  [computeContext->native setTexture:texture->native atIndex:CPSR_METAL_COMPUTE_SHADER_WRITE_TEXTURE_OFFSET + index];
}

void CpsrComputeContextDispatch(const CpsrComputeContext *computeContext, uint16_t x, uint16_t y, uint16_t z) {
  CPSR_ASSUME(computeContext);
  CPSR_ASSUME(computeContext->native);
  CPSR_ASSUME(x > 0);
  CPSR_ASSUME(y > 0);
  CPSR_ASSUME(z > 0);

  MTLSize threadGroupCount = MTLSizeMake(x, y, z);
  MTLSize threadCount = MTLSizeMake(16, 16, 1);
  [computeContext->native dispatchThreadgroups:threadGroupCount threadsPerThreadgroup:threadCount];
}

void CpsrComputeContextClose(const CpsrComputeContext *computeContext) {
  CPSR_ASSUME(computeContext);
  CPSR_ASSUME(computeContext->native);

  [computeContext->native endEncoding];
}

#ifndef NDEBUG

void _CpsrComputeContextPushDebugGroup(const CpsrComputeContext *computeContext, const char *groupName) {
  CPSR_ASSUME(computeContext);
  CPSR_ASSUME(groupName);
  
  NSString *nsGroupName = [[NSString alloc] initWithBytesNoCopy:(void *)groupName length:strlen(groupName) encoding:NSUTF8StringEncoding freeWhenDone:NO];
  [computeContext->native pushDebugGroup:nsGroupName];
  [nsGroupName release];
}

void _CpsrComputeContextPopDebugGroup(const CpsrComputeContext *computeContext) {
  CPSR_ASSUME(computeContext);
  
  [computeContext->native popDebugGroup];
}
#endif
