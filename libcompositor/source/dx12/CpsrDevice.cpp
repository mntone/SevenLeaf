#include "CpsrGraphics+Private.h"

#include <wrl/client.h>
#include <utility>

using Microsoft::WRL::ComPtr;

// ---
// Feature support
// ---
static inline bool GetTearingSupport(IDXGIFactory5* factory, bool* tearingSupport) {
  BOOL isTearingSupport;

  HRESULT hr = factory->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &isTearingSupport, sizeof(BOOL));
  if (FAILED(hr)) {
    return true;
  }

  *tearingSupport = isTearingSupport;
  return false;
}

static inline bool GetHasUnifiedMemory(ID3D12Device* device, bool* hasUnifiedMemory) {
  D3D12_FEATURE_DATA_ARCHITECTURE architechture;
  architechture.NodeIndex = 0;

  HRESULT hr =
      device->CheckFeatureSupport(D3D12_FEATURE_ARCHITECTURE, &architechture, sizeof(D3D12_FEATURE_DATA_ARCHITECTURE));
  if (FAILED(hr)) {
    return true;
  }

  *hasUnifiedMemory = architechture.UMA;
  return false;
}

// ---
// Init
// ---
static inline IDXGIFactory4* GetDXGIFactory4(uint8_t* factoryType) {
#ifdef NDEBUG
  UINT flags = 0;
  HRESULT hr = S_OK;
#else
  UINT flags = DXGI_CREATE_FACTORY_DEBUG;
  HRESULT hr = S_OK;

  ComPtr<ID3D12Debug> debugController;
  hr = D3D12GetDebugInterface(IID_PPV_ARGS(&debugController));
  if (SUCCEEDED(hr)) {
    debugController->EnableDebugLayer();
  }
#endif

  IDXGIFactory6* dxgiFactory6;
  hr = CreateDXGIFactory2(flags, IID_PPV_ARGS(&dxgiFactory6));
  if (SUCCEEDED(hr)) {
    *factoryType = 6;
    return dxgiFactory6;
  }

  IDXGIFactory5* dxgiFactory5;
  hr = CreateDXGIFactory2(flags, IID_PPV_ARGS(&dxgiFactory5));
  if (SUCCEEDED(hr)) {
    *factoryType = 5;
    return dxgiFactory5;
  }

  IDXGIFactory4* dxgiFactory4;
  hr = CreateDXGIFactory2(flags, IID_PPV_ARGS(&dxgiFactory4));
  if (SUCCEEDED(hr)) {
    *factoryType = 4;
    return dxgiFactory4;
  }

  return NULL;
}

static inline CpsrDevice* CpsrDeviceCreate(uint8_t factoryType,
                                           IDXGIFactory4* dxgiFactory,
                                           IDXGIAdapter3* dxgiAdapter3) {
  bool tearingSupport = false;
  if (factoryType >= 5 && GetTearingSupport(static_cast<IDXGIFactory5*>(dxgiFactory), &tearingSupport)) {
    return NULL;
  }

  ID3D12Device* d3d12Device;
  HRESULT hr = D3D12CreateDevice(dxgiAdapter3, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&d3d12Device));
  if (FAILED(hr)) {
    return NULL;
  }

  bool hasUnifiedMemory;
  if (GetHasUnifiedMemory(d3d12Device, &hasUnifiedMemory)) {
    return NULL;
  }

  CpsrDevice* device = CpsrAlloc(CpsrDevice);
  if (device) {
    device->tearingSupport = tearingSupport;
    device->hasUnifiedMemory = hasUnifiedMemory;
    device->renderTargetIncrementSize = d3d12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

    device->factoryType = factoryType;
    device->dxgiFactory4 = dxgiFactory;
    device->nativeAdapter = dxgiAdapter3;
    device->nativeDevice = d3d12Device;
  }
  return device;
}

static inline CpsrDevice* CpsrDeviceCreate(uint8_t factoryType,
                                           IDXGIFactory4* dxgiFactory,
                                           IDXGIAdapter1* dxgiAdapter1) {
  IDXGIAdapter3* dxgiAdapter3;
  HRESULT hr = dxgiAdapter1->QueryInterface(&dxgiAdapter3);
  dxgiAdapter1->Release();
  if (FAILED(hr)) {
    dxgiFactory->Release();
    return NULL;
  }

  return CpsrDeviceCreate(factoryType, dxgiFactory, dxgiAdapter3);
}

void CpsrEnumDevice(CpsrDeviceProcedure enumFunc) {
  HRESULT hr = S_OK;
  UINT adapterIndex = 0;

  uint8_t factoryType;
  ComPtr<IDXGIFactory4> dxgiFactory4 = std::move(GetDXGIFactory4(&factoryType));
  if (!dxgiFactory4) {
    return;
  }

  if (factoryType >= 6) {
    IDXGIFactory6* dxgiFactory6 = static_cast<IDXGIFactory6*>(dxgiFactory4.Get());

    ComPtr<IDXGIAdapter4> dxgiAdapter4;
    while (SUCCEEDED(hr = dxgiFactory6->EnumAdapterByGpuPreference(
                         adapterIndex, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&dxgiAdapter4)))) {
      dxgiFactory4->AddRef();
      CpsrDevice* device = CpsrDeviceCreate(factoryType, dxgiFactory4.Get(), dxgiAdapter4.Detach());
      enumFunc(device);

      ++adapterIndex;
    }
  } else {
    ComPtr<IDXGIAdapter1> dxgiAdapter1;
    while (SUCCEEDED(hr = dxgiFactory4->EnumAdapters1(adapterIndex, &dxgiAdapter1))) {
      dxgiFactory4->AddRef();
      CpsrDevice* device = CpsrDeviceCreate(factoryType, dxgiFactory4.Get(), dxgiAdapter1.Detach());
      enumFunc(device);

      ++adapterIndex;
    }
  }
}

CpsrDevice* CpsrDeviceGetDefault() {
  HRESULT hr = S_OK;

#ifdef NDEBUG
  UINT flags = 0;
#else
  UINT flags = DXGI_CREATE_FACTORY_DEBUG;
#endif

  uint8_t factoryType;
  IDXGIFactory4* dxgiFactory4 = GetDXGIFactory4(&factoryType);
  if (!dxgiFactory4) {
    return NULL;
  }

  if (factoryType >= 6) {
    IDXGIFactory6* dxgiFactory6 = static_cast<IDXGIFactory6*>(dxgiFactory4);
    ComPtr<IDXGIAdapter4> dxgiAdapter4;
    hr = dxgiFactory6->EnumAdapterByGpuPreference(0, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&dxgiAdapter4));
    if (FAILED(hr)) {
      dxgiFactory6->Release();
      return NULL;
    }

    CpsrDevice* device = CpsrDeviceCreate(factoryType, dxgiFactory4, dxgiAdapter4.Detach());
    return device;
  } else {
    ComPtr<IDXGIAdapter1> dxgiAdapter1;
    hr = dxgiFactory4->EnumAdapters1(0, &dxgiAdapter1);
    if (FAILED(hr)) {
      dxgiFactory4->Release();
      return NULL;
    }

    CpsrDevice* device = CpsrDeviceCreate(factoryType, dxgiFactory4, dxgiAdapter1.Detach());
    return device;
  }
}

CpsrDevice* CpsrDeviceGetLowPower() {
  HRESULT hr = S_OK;

#ifdef NDEBUG
  UINT flags = 0;
#else
  UINT flags = DXGI_CREATE_FACTORY_DEBUG;
#endif

  ComPtr<IDXGIFactory6> dxgiFactory;
  hr = CreateDXGIFactory2(flags, IID_PPV_ARGS(&dxgiFactory));
  if (FAILED(hr)) {
    return NULL;
  }

  ComPtr<IDXGIAdapter3> dxgiAdapter;
  hr = dxgiFactory->EnumAdapterByGpuPreference(0, DXGI_GPU_PREFERENCE_MINIMUM_POWER, IID_PPV_ARGS(&dxgiAdapter));
  if (FAILED(hr)) {
    return NULL;
  }

  CpsrDevice* device = CpsrDeviceCreate(6, dxgiFactory.Detach(), dxgiAdapter.Detach());
  return device;
}

void CpsrDeviceDestroy(CpsrDevice* device) {
  CPSR_ASSUME(device);

  device->nativeDevice->Release();
  device->nativeAdapter->Release();
  CpsrDealloc(device);
}

// ---
// Member functions
// ---
size_t CpsrDeviceGetName(const CpsrDevice* device, char* deviceName, size_t maxCount) {
  CPSR_ASSUME(device);

  DXGI_ADAPTER_DESC2 adapterDesc = {};

  HRESULT hr = device->nativeAdapter->GetDesc2(&adapterDesc);
  if (FAILED(hr)) {
    return 0;
  }

  if (deviceName) {
    return wcstombs(deviceName, adapterDesc.Description, maxCount);
  } else {
    return wcslen(adapterDesc.Description);
  }
}

uint64_t CpsrDeviceGetCurrentAllocatedSize(const CpsrDevice* device) {
  CPSR_ASSUME(device);

  DXGI_QUERY_VIDEO_MEMORY_INFO memoryInfo;
  HRESULT hr = device->nativeAdapter->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_LOCAL, &memoryInfo);
  if (FAILED(hr)) {
    return 0;
  }
  return memoryInfo.CurrentUsage;
}

bool CpsrDeviceIsUnifiedMemoryAccess(const CpsrDevice* device) {
  CPSR_ASSUME(device);

  return device->hasUnifiedMemory;
}

bool CpsrDeviceIsMultisampleSupport(const CpsrDevice* device, CpsrPixelFormat pixelFormat, uint8_t sampleCount) {
  CPSR_ASSUME(device);

  D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS multisampleQualityLevels;
  multisampleQualityLevels.Format = PixelFormatAsDx12Type(pixelFormat);
  multisampleQualityLevels.SampleCount = sampleCount;
  multisampleQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;

  HRESULT hr = device->nativeDevice->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
                                                         &multisampleQualityLevels,
                                                         sizeof(D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS));
  if (FAILED(hr)) {
    return false;
  }
  return multisampleQualityLevels.NumQualityLevels > 0;
}

CpsrDeviceCapabilities CpsrDeviceGetCapabilities(const CpsrDevice* device) {
  CpsrDeviceCapabilities capabilities;
  capabilities.bufferOffsetAlignment = D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT;
  capabilities.maximumRenderTargetCount = 8;
  return capabilities;
}
