#include "CpsrGraphics+Private.h"

CpsrFence *CpsrFenceCreate(const CpsrDevice *device, CpsrFenceType fenceType) {
  CPSR_ASSUME(device);

  ID3D12Fence *native;
  HRESULT hr = ID3D12Device_CreateFence(
      device->nativeDevice,
      0,
      fenceType == CPSR_FENCE_SHARED ? D3D12_FENCE_FLAG_SHARED_CROSS_ADAPTER : D3D12_FENCE_FLAG_SHARED,
      &IID_ID3D12Fence,
      &native);
  if (FAILED(hr)) {
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

void CpsrFenceDestroy(CpsrFence *fence) {
  CPSR_ASSUME(fence);

  ID3D12Fence_Release(fence->native);
  CpsrDealloc(fence);
}
