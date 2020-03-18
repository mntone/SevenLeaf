#ifndef _CPSR_GRAPHICS_PRIVATE_H
#define _CPSR_GRAPHICS_PRIVATE_H

#include "compositor/CpsrGraphics.h"

#ifndef STRICT
#define STRICT
#endif
#define WIN32_LEAN_AND_MEAN
#define NOSERVICE
#define NOMCX
#define NOIME
#include <Windows.h>

#include <assert.h>

#define COBJMACROS
#include <d3d12.h>
#include <dxgi1_6.h>
#undef COBJMACROS

#define CpsrAlloc(__TYPE__) (__TYPE__ *)malloc(sizeof(__TYPE__))
#define CpsrDealloc(__OBJ__) free((void *)__OBJ__)

#ifndef NDEBUG
#if defined(__clang__)
#define CPSR_ASSUME(__COND__) __builtin_assume(__COND__)
#elif defined(__GNUC__) || defined(__GNUG__)
#define CPSR_ASSUME(__COND__) \
  if (!(__COND__)) __builtin_unreachable()
#elif defined(_MSC_VER)
#define CPSR_ASSUME(__COND__) __assume(__COND__)
#else
#define CPSR_ASSUME(__COND__) assert(__COND__)
#endif
#else
#define CPSR_ASSUME(__COND__) assert(__COND__)
#endif

static inline wchar_t *CpsrGetUtf16String(const char *utf8String, int utf8Length, int *utf16Length) {
  int length = MultiByteToWideChar(CP_UTF8, 0, utf8String, utf8Length, NULL, 0);
  wchar_t *utf16String = (wchar_t *)malloc(sizeof(wchar_t) * length);
  if (!MultiByteToWideChar(CP_UTF8, 0, utf8String, utf8Length, utf16String, length)) {
    free(utf16String);
    return NULL;
  }
  if (utf16Length) {
    *utf16Length = length;
  }
  return utf16String;
}

// clang-format off
static inline DXGI_FORMAT PixelFormatAsDx12Type(CpsrPixelFormat pixelFormat) {
  switch (pixelFormat) {
  // 8-bit
  case CPSR_PIXELFORMAT_R8_UNORM:         return DXGI_FORMAT_R8_UNORM;
  case CPSR_PIXELFORMAT_R8_SNORM:         return DXGI_FORMAT_R8_SNORM;
  case CPSR_PIXELFORMAT_R8_UINT:          return DXGI_FORMAT_R8_UINT;
  case CPSR_PIXELFORMAT_R8_SINT:          return DXGI_FORMAT_R8_SINT;
  
  case CPSR_PIXELFORMAT_A8_UNORM:         return DXGI_FORMAT_A8_UNORM;
      
  // 16-bit
  case CPSR_PIXELFORMAT_R16_UNORM:        return DXGI_FORMAT_R16_UNORM;
  case CPSR_PIXELFORMAT_R16_SNORM:        return DXGI_FORMAT_R16_SNORM;
  case CPSR_PIXELFORMAT_R16_UINT:         return DXGI_FORMAT_R16_UINT;
  case CPSR_PIXELFORMAT_R16_SINT:         return DXGI_FORMAT_R16_SINT;
  case CPSR_PIXELFORMAT_R16_FLOAT:        return DXGI_FORMAT_R16_FLOAT;
  
  case CPSR_PIXELFORMAT_RG8_UNORM:        return DXGI_FORMAT_R8G8_UNORM;
  case CPSR_PIXELFORMAT_RG8_SNORM:        return DXGI_FORMAT_R8G8_SNORM;
  case CPSR_PIXELFORMAT_RG8_UINT:         return DXGI_FORMAT_R8G8_UINT;
  case CPSR_PIXELFORMAT_RG8_SINT:         return DXGI_FORMAT_R8G8_SINT;
      
  // 32-bit
  case CPSR_PIXELFORMAT_R32_UINT:         return DXGI_FORMAT_R32_UINT;
  case CPSR_PIXELFORMAT_R32_SINT:         return DXGI_FORMAT_R32_SINT;
  case CPSR_PIXELFORMAT_R32_FLOAT:        return DXGI_FORMAT_R32_FLOAT;

  case CPSR_PIXELFORMAT_RG16_UNORM:       return DXGI_FORMAT_R16G16_UNORM;
  case CPSR_PIXELFORMAT_RG16_SNORM:       return DXGI_FORMAT_R16G16_SNORM;
  case CPSR_PIXELFORMAT_RG16_UINT:        return DXGI_FORMAT_R16G16_UINT;
  case CPSR_PIXELFORMAT_RG16_SINT:        return DXGI_FORMAT_R16G16_SINT;
  case CPSR_PIXELFORMAT_RG16_FLOAT:       return DXGI_FORMAT_R16G16_FLOAT;
      
  case CPSR_PIXELFORMAT_RGBA8_UNORM:      return DXGI_FORMAT_R8G8B8A8_UNORM;
  case CPSR_PIXELFORMAT_RGBA8_UNORM_SRGB: return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
  case CPSR_PIXELFORMAT_RGBA8_SNORM:      return DXGI_FORMAT_R8G8B8A8_SNORM;
  case CPSR_PIXELFORMAT_RGBA8_UINT:       return DXGI_FORMAT_R8G8B8A8_UINT;
  case CPSR_PIXELFORMAT_RGBA8_SINT:       return DXGI_FORMAT_R8G8B8A8_SINT;

  case CPSR_PIXELFORMAT_BGRA8_UNORM:      return DXGI_FORMAT_B8G8R8A8_UNORM;
  case CPSR_PIXELFORMAT_BGRA8_UNORM_SRGB: return DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;

  case CPSR_PIXELFORMAT_RGB10A2_UNORM:    return DXGI_FORMAT_R10G10B10A2_UNORM;
  case CPSR_PIXELFORMAT_RGB10A2_UINT:     return DXGI_FORMAT_R10G10B10A2_UINT;

  // 64-bit
  case CPSR_PIXELFORMAT_RG32_UINT:        return DXGI_FORMAT_R32G32_UINT;
  case CPSR_PIXELFORMAT_RG32_SINT:        return DXGI_FORMAT_R32G32_SINT;
  case CPSR_PIXELFORMAT_RG32_FLOAT:       return DXGI_FORMAT_R32G32_FLOAT;

  case CPSR_PIXELFORMAT_RGBA16_UNORM:     return DXGI_FORMAT_R16G16B16A16_UNORM;
  case CPSR_PIXELFORMAT_RGBA16_SNORM:     return DXGI_FORMAT_R16G16B16A16_SNORM;
  case CPSR_PIXELFORMAT_RGBA16_UINT:      return DXGI_FORMAT_R16G16B16A16_UINT;
  case CPSR_PIXELFORMAT_RGBA16_SINT:      return DXGI_FORMAT_R16G16B16A16_SINT;
  case CPSR_PIXELFORMAT_RGBA16_FLOAT:     return DXGI_FORMAT_R16G16B16A16_FLOAT;
  
  default:                                return DXGI_FORMAT_UNKNOWN;
  }
}
// clang-format on

struct _CpsrDevice {
  bool tearingSupport;
  bool hasUnifiedMemory;
  uint32_t renderTargetIncrementSize;

  uint8_t factoryType;
  union {
    IDXGIFactory4 *dxgiFactory4;  // 1507 TH1 or later
    IDXGIFactory5 *dxgiFactory5;  // 1607 RS1 (Anniversary Update) or later
    IDXGIFactory6 *dxgiFactory6;  // 1803 RS4 (April 2018 Update) or later
  };
  IDXGIAdapter3 *nativeAdapter;
  ID3D12Device *nativeDevice;
};

struct _CpsrFence {
  ID3D12Fence *native;

#ifndef NDEBUG
  const CpsrDevice *device;
  CpsrFenceType fenceType;
#endif
};

struct _CpsrBuffer {
  ID3D12Resource *native;
  size_t size;

#ifndef NDEBUG
  const CpsrDevice *device;
  CpsrHeapType heapType;
  CpsrBufferType bufferType;
#endif
};

struct _CpsrTexture2D {
  const CpsrDevice *device;
  ID3D12Resource *nativeResource;
  ID3D12DescriptorHeap *nativeHeap;
  D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc;

#ifndef NDEBUG
  CpsrHeapType heapType;
  CpsrTextureUsage usage;
#endif
};

size_t CpsrTexture2DGetNativeHandle(const CpsrTexture2D *texture2D);

struct _CpsrSwapChain {
  const CpsrCommandQueue *graphicsCommandQueue;
  DXGI_FORMAT pixelFormat;
  CpsrSizeU32 size;
  uint8_t bufferCount;
  size_t currentIndex;

  IDXGISwapChain3 *native;
  ID3D12DescriptorHeap *heap;
  ID3D12Resource *renderTargets[3];
};

ID3D12Resource *CpsrSwapChainGetNativeRenderTargetResource(const CpsrSwapChain *swapChain);
size_t CpsrSwapChainGetNativeRenderTargetHandle(const CpsrSwapChain *swapChain);

struct _CpsrShaderLibrary {
  const CpsrDevice *device;
  const char *filepath;
  size_t filepathLength;
};

struct _CpsrShaderFunction {
  ID3DBlob *native;
};

struct _CpsrCommandQueue {
  const CpsrDevice *device;
  ID3D12CommandQueue *native;
};

struct _CpsrCommandBuffer {
  const CpsrCommandQueue *commandQueue;
  ID3D12CommandAllocator *native;
};

ID3D12PipelineState *CpsrGraphicsPipelineStateGetNative(CpsrGraphicsPipelineState *pipelineState);
ID3D12PipelineState *CpsrComputePipelineStateGetNative(CpsrComputePipelineState *pipelineState);

struct _CpsrGraphicsContext {
  const CpsrCommandBuffer *commandBuffer;
  ID3D12GraphicsCommandList *native;
};

struct _CpsrComputeContext {
  const CpsrCommandBuffer *commandBuffer;
  ID3D12GraphicsCommandList *native;
};

#endif  // _CPSR_GRAPHICS_PRIVATE_H
