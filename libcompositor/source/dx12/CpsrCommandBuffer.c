#include "CpsrGraphics+Private.h"

CpsrCommandBuffer *CpsrCommandBufferCreate(const CpsrCommandQueue *commandQueue) {
  CPSR_ASSUME(commandQueue);

  ID3D12CommandAllocator *native;
  HRESULT hr = ID3D12Device_CreateCommandAllocator(
      commandQueue->device->nativeDevice, D3D12_COMMAND_LIST_TYPE_DIRECT, &IID_ID3D12CommandAllocator, &native);
  if (FAILED(hr)) {
    return NULL;
  }

  CpsrCommandBuffer *commandBuffer = CpsrAlloc(CpsrCommandBuffer);
  if (commandBuffer) {
    commandBuffer->native = native;
  }
  return commandBuffer;
}

void CpsrCommandBufferDestroy(CpsrCommandBuffer *commandBuffer) {
  CPSR_ASSUME(commandBuffer);

  ID3D12CommandAllocator_Release(commandBuffer->native);
  CpsrDealloc(commandBuffer);
}

void CpsrCommandBufferExecute(const CpsrCommandBuffer *commandBuffer) {
  CPSR_ASSUME(commandBuffer);

  //ID3D12CommandQueue_ExecuteCommandLists(commandBuffer->native, , );
  ID3D12CommandAllocator_Reset(commandBuffer->native);
}

void CpsrCommandBufferExecuteAndPresent(const CpsrCommandBuffer *commandBuffer, const CpsrSwapChain *swapChain) {
  CPSR_ASSUME(commandBuffer);
  CPSR_ASSUME(swapChain);

  //[commandBuffer->native presentDrawable:swapChain->currentDrawable];
  // ID3D12CommandQueue_ExecuteCommandLists(commandBuffer->native, , );
  ID3D12CommandAllocator_Reset(commandBuffer->native);
}

void CpsrCommandBufferSingle(const CpsrCommandBuffer *commandBuffer, const CpsrFence *fence, uint64_t value) {
  CPSR_ASSUME(commandBuffer);
  CPSR_ASSUME(fence);

  ID3D12CommandQueue_Signal(commandBuffer->commandQueue->native, fence->native, value);
}

void CpsrCommandBufferWait(const CpsrCommandBuffer *commandBuffer, const CpsrFence *fence, uint64_t value) {
  CPSR_ASSUME(commandBuffer);
  CPSR_ASSUME(fence);

  ID3D12CommandQueue_Wait(commandBuffer->commandQueue->native, fence->native, value);
}

#ifndef NDEBUG
// Ref: pix.h
static const UINT PIX_EVENT_UNICODE_VERSION = 0;

void _CpsrCommandBufferPushDebugGroup(const CpsrCommandBuffer *commandBuffer, const char *groupName) {
  CPSR_ASSUME(commandBuffer);
  CPSR_ASSUME(groupName);

  int utf16GroupNameLength = 0;
  wchar_t *utf16GroupName = CpsrGetUtf16String(groupName, strlen(groupName), &utf16GroupNameLength);
  UINT size = (UINT)(sizeof(wchar_t) * (utf16GroupNameLength + 1));
  ID3D12CommandQueue_BeginEvent(commandBuffer->commandQueue->native, PIX_EVENT_UNICODE_VERSION, utf16GroupName, size);
  free(utf16GroupName);
}

void _CpsrCommandBufferPopDebugGroup(const CpsrCommandBuffer *commandBuffer) {
  CPSR_ASSUME(commandBuffer);

  ID3D12CommandQueue_EndEvent(commandBuffer->commandQueue->native);
}
#endif
