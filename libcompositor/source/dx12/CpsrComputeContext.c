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
    ID3D12GraphicsCommandList_Release(computeContext->native);
  }
  CpsrDealloc(computeContext);
}

bool CpsrComputeContextSetPipelineState(CpsrComputeContext *computeContext, CpsrComputePipelineState *pipelineState) {
  assert(computeContext);
  assert(!computeContext->native);
  assert(pipelineState);

  ID3D12PipelineState *nativePipelineState = CpsrComputePipelineStateGetNative(pipelineState);
  if (!nativePipelineState) {
    // TODO: logo
    return true;
  }

  ID3D12GraphicsCommandList *commandList;
  HRESULT hr = ID3D12Device_CreateCommandList(computeContext->commandBuffer->commandQueue->device->nativeDevice,
                                              0,
                                              D3D12_COMMAND_LIST_TYPE_DIRECT,
                                              computeContext->commandBuffer->native,
                                              nativePipelineState,
                                              &IID_ID3D12GraphicsCommandList,
                                              &commandList);
  if (FAILED(hr)) {
    return true;
  }

  computeContext->native = commandList;
  return false;
}

void CpsrComputeContextSetConstantBuffer(CpsrComputeContext *computeContext,
                                         uint8_t index,
                                         const CpsrBuffer *vertexBuffer) {
  CPSR_ASSUME(computeContext);
  CPSR_ASSUME(computeContext->native);
  CPSR_ASSUME(vertexBuffer);
  CPSR_ASSUME(vertexBuffer->bufferType == CPSR_CONSTANT_BUFFER);

  D3D12_GPU_VIRTUAL_ADDRESS location = ID3D12Resource_GetGPUVirtualAddress(vertexBuffer->native);
  ID3D12GraphicsCommandList_SetComputeRootConstantBufferView(computeContext->native, index, location);
}

void CpsrComputeContextSetReadTexture(CpsrComputeContext *computeContext, uint8_t index, const CpsrTexture2D *texture) {
  CPSR_ASSUME(computeContext);
  CPSR_ASSUME(computeContext->native);
  CPSR_ASSUME(index < 16);
  CPSR_ASSUME(texture);
  CPSR_ASSUME(texture->usage & CPSR_TEXTURE_USAGE_READ);

  D3D12_GPU_VIRTUAL_ADDRESS location = ID3D12Resource_GetGPUVirtualAddress(texture->nativeResource);
  ID3D12GraphicsCommandList_SetComputeRootShaderResourceView(computeContext->native, index, location);
}

void CpsrComputeContextSetWriteTexture(CpsrComputeContext *computeContext,
                                       uint8_t index,
                                       const CpsrTexture2D *texture) {
  CPSR_ASSUME(computeContext);
  CPSR_ASSUME(computeContext->native);
  CPSR_ASSUME(index < 16);
  CPSR_ASSUME(texture);
  CPSR_ASSUME(texture->usage & CPSR_TEXTURE_USAGE_WRITE);

  D3D12_GPU_VIRTUAL_ADDRESS location = ID3D12Resource_GetGPUVirtualAddress(texture->nativeResource);
  ID3D12GraphicsCommandList_SetComputeRootUnorderedAccessView(computeContext->native, index, location);
}

void CpsrComputeContextDispatch(const CpsrComputeContext *computeContext, uint16_t x, uint16_t y, uint16_t z) {
  CPSR_ASSUME(computeContext);
  CPSR_ASSUME(computeContext->native);
  CPSR_ASSUME(x > 0);
  CPSR_ASSUME(y > 0);
  CPSR_ASSUME(z > 0);

  ID3D12GraphicsCommandList_Dispatch(computeContext->native, x, y, z);
}

void CpsrComputeContextClose(const CpsrComputeContext *computeContext) {
  CPSR_ASSUME(computeContext);
  CPSR_ASSUME(computeContext->native);

  ID3D12GraphicsCommandList_Close(computeContext->native);
}

#ifndef NDEBUG
// Ref: pix.h
static const UINT PIX_EVENT_UNICODE_VERSION = 0;

void _CpsrComputeContextPushDebugGroup(const CpsrComputeContext *computeContext, const char *groupName) {
  CPSR_ASSUME(computeContext);
  CPSR_ASSUME(groupName);

  int utf16GroupNameLength = 0;
  wchar_t *utf16GroupName = CpsrGetUtf16String(groupName, (int)strlen(groupName), &utf16GroupNameLength);
  UINT size = (UINT)(sizeof(wchar_t) * (utf16GroupNameLength + 1));
  ID3D12GraphicsCommandList_BeginEvent(computeContext->native, PIX_EVENT_UNICODE_VERSION, utf16GroupName, size);
  free(utf16GroupName);
}

void _CpsrComputeContextPopDebugGroup(const CpsrComputeContext *computeContext) {
  CPSR_ASSUME(computeContext);

  ID3D12GraphicsCommandList_EndEvent(computeContext->native);
}
#endif
