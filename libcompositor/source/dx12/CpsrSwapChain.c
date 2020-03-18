#include "CpsrGraphics+Private.h"

static inline void SwapChainResourceCreate(CpsrSwapChain *swapChain) {
  const uint8_t frameCount = swapChain->bufferCount;

  ID3D12Device *device;
  device = swapChain->graphicsCommandQueue->device->nativeDevice;
  if (!device) {
    return NULL;
  }
  ID3D12Device_AddRef(device);

  D3D12_DESCRIPTOR_HEAP_DESC heapDesc;
  heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
  heapDesc.NumDescriptors = frameCount;
  heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
  heapDesc.NodeMask = 0;

  ID3D12DescriptorHeap *heap;
  HRESULT hr = ID3D12Device_CreateDescriptorHeap(device, &heapDesc, &IID_ID3D12DescriptorHeap, &heap);
  if (FAILED(hr)) {
    ID3D12Device_Release(device);
    return NULL;
  }
  swapChain->heap = heap;

  bool error = false;
  D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = ID3D12DescriptorHeap_GetCPUDescriptorHandleForHeapStart(heap);
  for (size_t i = 0; i < frameCount; ++i) {
    ID3D12Resource *renderTarget;
    hr = IDXGISwapChain1_GetBuffer(swapChain->native, i, &IID_ID3D12Resource, &renderTarget);
    if (FAILED(hr)) {
      error = true;
      continue;
    }

    ID3D12Device_CreateRenderTargetView(device, renderTarget, NULL, cpuHandle);
    swapChain->renderTargets[i] = renderTarget;

    cpuHandle.ptr += swapChain->graphicsCommandQueue->device->renderTargetIncrementSize;
  }

  ID3D12Device_Release(device);
  return error;
}

static inline void SwapChainResourceRelease(CpsrSwapChain *swapChain) {
  const uint8_t frameCount = swapChain->bufferCount;
  for (size_t i = 0; i < frameCount; ++i) {
    ID3D12Resource *renderTarget = swapChain->renderTargets[i];
    if (renderTarget) {
      swapChain->renderTargets[i] = NULL;

      ID3D12Resource_Release(renderTarget);
    }
  }

  ID3D12DescriptorHeap *heap = swapChain->heap;
  if (heap) {
    swapChain->heap = NULL;

    ID3D12DescriptorHeap_Release(heap);
  }
}

CpsrSwapChain *CpsrSwapChainCreate(const CpsrCommandQueue *graphicsCommandQueue,
                                   CpsrViewHost viewHost,
                                   uint8_t bufferCount,
                                   bool vsyncEnable) {
  CPSR_ASSUME(graphicsCommandQueue);
  CPSR_ASSUME(bufferCount >= 2 && bufferCount <= 3);

  DXGI_FORMAT nativePixelFormat = PixelFormatAsDx12Type(viewHost.pixelFormat);

  DXGI_SWAP_CHAIN_DESC1 desc;
  desc.Width = viewHost.size.width;
  desc.Height = viewHost.size.height;
  desc.Format = nativePixelFormat;
  desc.Stereo = FALSE;
  desc.SampleDesc.Count = 1;
  desc.SampleDesc.Quality = 0;
  desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
  desc.BufferCount = bufferCount;
  desc.Scaling = DXGI_SCALING_STRETCH;
  desc.SwapEffect = vsyncEnable ? DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL : DXGI_SWAP_EFFECT_FLIP_DISCARD;
  desc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
  desc.Flags = 0;

  IDXGIFactory4 *dxgiFactory;
  HRESULT hr = CreateDXGIFactory1(&IID_IDXGIFactory4, &dxgiFactory);
  if (FAILED(hr)) {
    return;
  }

  IDXGISwapChain1 *swapChain1;
  hr = IDXGIFactory4_CreateSwapChainForHwnd(
      dxgiFactory, graphicsCommandQueue->native, (HWND)viewHost.handle, &desc, NULL, NULL, &swapChain1);
  if (FAILED(hr)) {
    IDXGIFactory4_Release(dxgiFactory);
    return;
  }

  hr = IDXGIFactory4_MakeWindowAssociation(dxgiFactory, (HWND)viewHost.handle, DXGI_MWA_NO_ALT_ENTER);
  IDXGIFactory4_Release(dxgiFactory);
  if (FAILED(hr)) {
    IDXGISwapChain1_Release(swapChain1);
    return;
  }

  IDXGISwapChain3 *swapChain3;
  hr = IDXGISwapChain1_QueryInterface(swapChain1, &IID_IDXGISwapChain3, &swapChain3);
  IDXGISwapChain1_Release(swapChain1);
  if (FAILED(hr)) {
    IDXGIFactory4_Release(dxgiFactory);
    return;
  }

  hr = IDXGISwapChain3_SetMaximumFrameLatency(swapChain3, 1);
  if (FAILED(hr)) {
    IDXGISwapChain3_Release(swapChain3);
    IDXGIFactory4_Release(dxgiFactory);
    return;
  }

  CpsrSwapChain *swapChain = CpsrAlloc(CpsrSwapChain);
  if (swapChain) {
    swapChain->graphicsCommandQueue = graphicsCommandQueue;
    swapChain->pixelFormat = nativePixelFormat;
    swapChain->size = viewHost.size;
    swapChain->bufferCount = bufferCount;
    swapChain->currentIndex = 0;
    swapChain->native = swapChain3;
    swapChain->renderTargets[0] = NULL;
    swapChain->renderTargets[1] = NULL;
    swapChain->renderTargets[2] = NULL;

    SwapChainResourceCreate(swapChain);
  }
  return swapChain;
}

void CpsrSwapChainDestroy(CpsrSwapChain *swapChain) {
  CPSR_ASSUME(swapChain);

  SwapChainResourceRelease(swapChain);
  IDXGISwapChain1_Release(swapChain->native);
  CpsrDealloc(swapChain);
}

bool CpsrSwapChainNextBuffer(CpsrSwapChain *swapChain) {
  CPSR_ASSUME(swapChain);

  size_t currentIndex = swapChain->currentIndex + 1;
  if (currentIndex % swapChain->bufferCount == 0) {
    currentIndex = 0;
  }
  swapChain->currentIndex = currentIndex;
  return false;
}

void CpsrSwapChainChangeSize(CpsrSwapChain *swapChain, CpsrSizeU32 size) {
  CPSR_ASSUME(swapChain);

  SwapChainResourceRelease(swapChain);

  swapChain->size = size;
  IDXGISwapChain1_ResizeBuffers(
      swapChain->native, swapChain->bufferCount, size.width, size.height, swapChain->pixelFormat, 0);

  SwapChainResourceCreate(swapChain);
}

void CpsrSwapChainChangeBufferCount(CpsrSwapChain *swapChain, uint8_t bufferCount) {
  CPSR_ASSUME(swapChain);
  CPSR_ASSUME(bufferCount >= 2 && bufferCount <= 3);

  SwapChainResourceRelease(swapChain);

  swapChain->bufferCount = bufferCount;
  IDXGISwapChain1_ResizeBuffers(
      swapChain->native, bufferCount, swapChain->size.width, swapChain->size.height, swapChain->pixelFormat, 0);

  SwapChainResourceCreate(swapChain);
}

// ---
// Internal functions
// ---
ID3D12Resource *CpsrSwapChainGetNativeRenderTargetResource(const CpsrSwapChain *swapChain) {
  CPSR_ASSUME(swapChain);

  return swapChain->renderTargets[swapChain->currentIndex];
}

size_t CpsrSwapChainGetNativeRenderTargetHandle(const CpsrSwapChain *swapChain) {
  CPSR_ASSUME(swapChain);

  D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = ID3D12DescriptorHeap_GetCPUDescriptorHandleForHeapStart(swapChain->heap);
  return cpuHandle.ptr + swapChain->graphicsCommandQueue->device->renderTargetIncrementSize * swapChain->currentIndex;
}
