#include "CpsrGraphics+Private.h"

CpsrCommandQueue *CpsrCommandQueueCreate(const CpsrDevice *device) {
  CPSR_ASSUME(device);

  D3D12_COMMAND_QUEUE_DESC desc;
  desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
  desc.Priority = 0;
  desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
  desc.NodeMask = 1;

  ID3D12CommandQueue *native;
  HRESULT hr = ID3D12Device_CreateCommandQueue(device->nativeDevice, &desc, &IID_ID3D12CommandQueue, &native);
  if (FAILED(hr)) {
    return NULL;
  }

  CpsrCommandQueue *commandQueue = CpsrAlloc(CpsrCommandQueue);
  if (commandQueue) {
    commandQueue->device = device;
    commandQueue->native = native;
  }
  return commandQueue;
}

void CpsrCommandQueueDestroy(CpsrCommandQueue *commandQueue) {
  CPSR_ASSUME(commandQueue);

  ID3D12CommandQueue_Release(commandQueue->native);
  CpsrDealloc(commandQueue);
}
