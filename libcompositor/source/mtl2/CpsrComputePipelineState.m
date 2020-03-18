#include "CpsrGraphics+Private.h"

CpsrComputePipelineState *CpsrComputePipelineStateCreate(const CpsrDevice *device) {
  CPSR_ASSUME(device);

  CpsrComputePipelineState *pipelineState = CpsrAlloc(CpsrComputePipelineState);
  if (!pipelineState) {
    return NULL;
  }

  pipelineState->device   = device;
  pipelineState->function = NULL;
  pipelineState->native   = NULL;
  return pipelineState;
}

static inline void CpsrComputePipelineStateRelease(CpsrComputePipelineState *pipelineState) {
  id<MTLComputePipelineState> native = pipelineState->native;
  if (native) {
    [native release];
    pipelineState->native = nil;
  }
}

void CpsrComputePipelineStateDestroy(CpsrComputePipelineState *pipelineState) {
  CPSR_ASSUME(pipelineState);

  CpsrComputePipelineStateRelease(pipelineState);
  CpsrDealloc(pipelineState);
}

id<MTLComputePipelineState> CpsrComputePipelineStateGetNative(CpsrComputePipelineState *pipelineState) {
  id<MTLComputePipelineState> native = pipelineState->native;
  if (!native) {
    NSError *error = nil;
    native = [pipelineState->device->native newComputePipelineStateWithFunction:pipelineState->function->native error:&error];
    
    if (error) {
      // TODO: log
      [error release];
    } else {
      pipelineState->native = native;
    }
  }
  return native;
}

// ---
// Property: Compute Function
// ---
void CpsrComputePipelineStateSetFunction(CpsrComputePipelineState *pipelineState,
                                         const CpsrShaderFunction *shaderFunction) {
  CPSR_ASSUME(pipelineState);
  CPSR_ASSUME(shaderFunction);
  
  if (pipelineState->function != shaderFunction) {
    CpsrComputePipelineStateRelease(pipelineState);
    
    pipelineState->function = shaderFunction;
  }
}
