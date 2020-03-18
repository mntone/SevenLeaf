#include "CpsrGraphics+Private.h"

struct _CpsrComputePipelineState {
  const CpsrDevice *device;
  D3D12_COMPUTE_PIPELINE_STATE_DESC desc;
  ID3D12PipelineState *native;
};

CpsrComputePipelineState *CpsrComputePipelineStateCreate(const CpsrDevice *device) {
  CPSR_ASSUME(device);

  CpsrComputePipelineState *pipelineState = CpsrAlloc(CpsrComputePipelineState);
  if (!pipelineState) {
    return NULL;
  }

  pipelineState->device = device;
  pipelineState->desc.pRootSignature = NULL;
  pipelineState->desc.CS.pShaderBytecode = NULL;
  pipelineState->desc.CS.BytecodeLength = 0;
  pipelineState->desc.NodeMask = 0;
  pipelineState->desc.CachedPSO.pCachedBlob = NULL;
  pipelineState->desc.CachedPSO.CachedBlobSizeInBytes = 0;
  pipelineState->desc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
  pipelineState->native = NULL;
  return pipelineState;
}

static inline void CpsrComputePipelineStateRelease(CpsrComputePipelineState *pipelineState) {
  ID3D12PipelineState *native = pipelineState->native;
  if (native) {
    ID3D12PipelineState_Release(native);
    pipelineState->native = NULL;
  }
}

void CpsrComputePipelineStateDestroy(CpsrComputePipelineState *pipelineState) {
  CPSR_ASSUME(pipelineState);

  CpsrComputePipelineStateRelease(pipelineState);
  CpsrDealloc(pipelineState);
}

ID3D12PipelineState *CpsrComputePipelineStateGetNative(CpsrComputePipelineState *pipelineState) {
  ID3D12PipelineState *native = pipelineState->native;
  if (!native) {
    HRESULT hr = ID3D12Device_CreateComputePipelineState(
        pipelineState->device->nativeDevice, &pipelineState->desc, &IID_ID3D12PipelineState, &native);
    if (FAILED(hr)) {
      // TODO: log
    }

    pipelineState->native = native;
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
  CpsrComputePipelineStateRelease(pipelineState);

  if (shaderFunction) {
    pipelineState->desc.CS.pShaderBytecode = ID3D10Blob_GetBufferPointer(shaderFunction->native);
    pipelineState->desc.CS.BytecodeLength = ID3D10Blob_GetBufferSize(shaderFunction->native);
  }
}
