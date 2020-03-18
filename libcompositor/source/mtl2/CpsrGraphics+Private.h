#ifndef _CPSR_GRAPHICS_PRIVATE_H
#define _CPSR_GRAPHICS_PRIVATE_H

#include "compositor/CpsrGraphics.h"

#include <assert.h>
#import <Metal/Metal.h>

#define CpsrAlloc(__TYPE__) (__TYPE__ *)malloc(sizeof(__TYPE__))
#define CpsrDealloc(__OBJ__) free((void *)__OBJ__)

#ifndef NDEBUG
#if defined(__clang__)
#define CPSR_ASSUME(__COND__) __builtin_assume(__COND__)
#elif defined(__GNUC__) || defined(__GNUG__)
#define CPSR_ASSUME(__COND__) if (!(__COND__)) __builtin_unreachable()
#elif defined(_MSC_VER)
#define CPSR_ASSUME(__COND__) __assume(__COND__)
#else
#define CPSR_ASSUME(__COND__) assert(__COND__)
#endif
#else
#define CPSR_ASSUME(__COND__) assert(__COND__)
#endif

struct _CpsrDevice {
  bool hasUnifiedMemory;
  id<MTLDevice> native;
};

static inline MTLPixelFormat PixelFormatAsMetalType(CpsrPixelFormat pixelFormat) {
  switch (pixelFormat) {
  // 8-bit
  case CPSR_PIXELFORMAT_R8_UNORM:         return MTLPixelFormatR8Unorm;
  case CPSR_PIXELFORMAT_R8_SNORM:         return MTLPixelFormatR8Snorm;
  case CPSR_PIXELFORMAT_R8_UINT:          return MTLPixelFormatR8Uint;
  case CPSR_PIXELFORMAT_R8_SINT:          return MTLPixelFormatR8Sint;
  
  case CPSR_PIXELFORMAT_A8_UNORM:         return MTLPixelFormatA8Unorm;
      
  // 16-bit
  case CPSR_PIXELFORMAT_R16_UNORM:        return MTLPixelFormatR16Unorm;
  case CPSR_PIXELFORMAT_R16_SNORM:        return MTLPixelFormatR16Snorm;
  case CPSR_PIXELFORMAT_R16_UINT:         return MTLPixelFormatR16Uint;
  case CPSR_PIXELFORMAT_R16_SINT:         return MTLPixelFormatR16Sint;
  case CPSR_PIXELFORMAT_R16_FLOAT:        return MTLPixelFormatR16Float;
  
  case CPSR_PIXELFORMAT_RG8_UNORM:        return MTLPixelFormatRG8Unorm;
  case CPSR_PIXELFORMAT_RG8_SNORM:        return MTLPixelFormatRG8Snorm;
  case CPSR_PIXELFORMAT_RG8_UINT:         return MTLPixelFormatRG8Uint;
  case CPSR_PIXELFORMAT_RG8_SINT:         return MTLPixelFormatRG8Sint;
  
#if TARGET_OS_IOS
  case CPSR_PIXELFORMAT_ABGR4_UNORM:      return MTLPixelFormatABGR4Unorm;
  case CPSR_PIXELFORMAT_B5G6R5_UNORM:     return MTLPixelFormatB5G6R5Unorm;
  case CPSR_PIXELFORMAT_A1BGR5_UNORM:     return MTLPixelFormatA1BGR5Unorm;
  case CPSR_PIXELFORMAT_BGR5A1_UNORM:     return MTLPixelFormatBGR5A1Unorm;
#endif
      
  // 32-bit
  case CPSR_PIXELFORMAT_R32_UINT:         return MTLPixelFormatR32Uint;
  case CPSR_PIXELFORMAT_R32_SINT:         return MTLPixelFormatR32Sint;
  case CPSR_PIXELFORMAT_R32_FLOAT:        return MTLPixelFormatR32Float;

  case CPSR_PIXELFORMAT_RG16_UNORM:       return MTLPixelFormatRG16Unorm;
  case CPSR_PIXELFORMAT_RG16_SNORM:       return MTLPixelFormatRG16Snorm;
  case CPSR_PIXELFORMAT_RG16_UINT:        return MTLPixelFormatRG16Uint;
  case CPSR_PIXELFORMAT_RG16_SINT:        return MTLPixelFormatRG16Sint;
  case CPSR_PIXELFORMAT_RG16_FLOAT:       return MTLPixelFormatRG16Float;
      
  case CPSR_PIXELFORMAT_RGBA8_UNORM:      return MTLPixelFormatRGBA8Unorm;
  case CPSR_PIXELFORMAT_RGBA8_UNORM_SRGB: return MTLPixelFormatRGBA8Unorm_sRGB;
  case CPSR_PIXELFORMAT_RGBA8_SNORM:      return MTLPixelFormatRGBA8Snorm;
  case CPSR_PIXELFORMAT_RGBA8_UINT:       return MTLPixelFormatRGBA8Uint;
  case CPSR_PIXELFORMAT_RGBA8_SINT:       return MTLPixelFormatRGBA8Sint;

  case CPSR_PIXELFORMAT_BGRA8_UNORM:      return MTLPixelFormatBGRA8Unorm;
  case CPSR_PIXELFORMAT_BGRA8_UNORM_SRGB: return MTLPixelFormatBGRA8Unorm_sRGB;

  case CPSR_PIXELFORMAT_RGB10A2_UNORM:    return MTLPixelFormatRGB10A2Unorm;
  case CPSR_PIXELFORMAT_RGB10A2_UINT:     return MTLPixelFormatRGB10A2Uint;
      
  case CPSR_PIXELFORMAT_BGR10A2_UNORM:    return MTLPixelFormatBGR10A2Unorm;

  // 64-bit
  case CPSR_PIXELFORMAT_RG32_UINT:        return MTLPixelFormatRG32Uint;
  case CPSR_PIXELFORMAT_RG32_SINT:        return MTLPixelFormatRG32Sint;
  case CPSR_PIXELFORMAT_RG32_FLOAT:       return MTLPixelFormatRG32Float;

  case CPSR_PIXELFORMAT_RGBA16_UNORM:     return MTLPixelFormatRGBA16Unorm;
  case CPSR_PIXELFORMAT_RGBA16_SNORM:     return MTLPixelFormatRGBA16Snorm;
  case CPSR_PIXELFORMAT_RGBA16_UINT:      return MTLPixelFormatRGBA16Uint;
  case CPSR_PIXELFORMAT_RGBA16_SINT:      return MTLPixelFormatRGBA16Sint;
  case CPSR_PIXELFORMAT_RGBA16_FLOAT:     return MTLPixelFormatRGBA16Float;
  
  default:                                return MTLPixelFormatInvalid;
  }
}

static inline MTLResourceOptions HeapTypeAsMetalType(const CpsrDevice *device, CpsrHeapType type) {
  bool upload = type & CPSR_HEAP_TYPE_UPLOAD;
  bool readback = type & CPSR_HEAP_TYPE_READBACK;
  MTLStorageMode interlopStorageMode = device->hasUnifiedMemory ? MTLResourceStorageModeShared : MTLResourceStorageModeManaged;
  
  MTLResourceOptions options = 0;
  if (readback) {
    options = interlopStorageMode;
  } else if (upload) {
    options = interlopStorageMode | MTLResourceCPUCacheModeWriteCombined;
  } else {
    options = MTLResourceStorageModePrivate;
  }
  
  if (@available(iOS 13.0, macOS 10.15, *)) {
    options |= MTLResourceHazardTrackingModeUntracked;
  }
  return options;
}

static inline MTLResourceUsage ResourceUsageAsMetalType(CpsrTextureUsage usage) {
  MTLResourceUsage resourceUsage = MTLResourceUsageRead;
  if (!(usage & CPSR_TEXTURE_USAGE_READ)) {
    resourceUsage ^= MTLResourceUsageRead;
  }
  if (usage & CPSR_TEXTURE_USAGE_WRITE) {
    resourceUsage |= MTLResourceUsageWrite;
  }
  if (usage & CPSR_TEXTURE_USAGE_RENDER_TARGET) {
    resourceUsage |= MTLTextureUsageRenderTarget;
  }
  return resourceUsage;
}

static inline void Texture2DDescriptorToMetalType(const CpsrTexture2DDescriptor *cpsrDesc, MTLTextureDescriptor *nativeDesc) {
  if (cpsrDesc->arrayLength > 1) {
    nativeDesc.textureType = MTLTextureType2DArray;
    nativeDesc.arrayLength = cpsrDesc->arrayLength;
  } else {
    nativeDesc.textureType = MTLTextureType2D;
    nativeDesc.arrayLength = 1;
  }
  nativeDesc.pixelFormat = PixelFormatAsMetalType(cpsrDesc->pixelFormat);
  nativeDesc.width = cpsrDesc->size.width;
  nativeDesc.height = cpsrDesc->size.height;
  nativeDesc.usage = ResourceUsageAsMetalType(cpsrDesc->usage);
}

static inline MTLTextureDescriptor *Texture2DDescriptorAsMetalType(const CpsrTexture2DDescriptor *cpsrDesc) {
  MTLTextureDescriptor *nativeDesc = [[MTLTextureDescriptor alloc] init];
  Texture2DDescriptorToMetalType(cpsrDesc, nativeDesc);

  // default: MTLHazardTrackingModeDefault
  if (@available(iOS 13.0, macOS 10.15, *)) {
    nativeDesc.hazardTrackingMode = MTLHazardTrackingModeUntracked;
  }
  return nativeDesc;
}

struct _CpsrFence {
  id<MTLEvent> native;
  
#ifndef NDEBUG
  const CpsrDevice *device;
  CpsrFenceType fenceType;
#endif
};

struct _CpsrHeap {
  const CpsrDevice *device;
  id<MTLHeap> native;
  CpsrHeapType heapType : 2;
};

struct _CpsrBuffer {
  id<MTLBuffer> native;
  
#ifndef NDEBUG
  const CpsrDevice *device;
  CpsrHeapType heapType : 2;
  CpsrBufferType bufferType : 3;
#endif
};

struct _CpsrTexture2D {
  id<MTLTexture> native;
    
#ifndef NDEBUG
  const CpsrDevice *device;
  CpsrHeapType heapType : 2;
  CpsrTextureUsage usage : 3;
#endif
};

#ifdef USE_SWAPCHAIN_PRIVATE_DEFINE
struct _CpsrSwapChain {
  const CpsrCommandQueue *graphicsCommandQueue;
  CAMetalLayer *metalLayer;
  CpsrViewHandle hostView;
  id<CAMetalDrawable> currentDrawable;
};
#endif

id<MTLTexture> CpsrSwapChainGetNativeTexture(const CpsrSwapChain *swapChain);

struct _CpsrShaderLibrary {
  const CpsrDevice *device;
  id<MTLLibrary> native;
};

struct _CpsrShaderFunction {
  id<MTLFunction> native;
};

struct _CpsrCommandQueue {
  const CpsrDevice *device;
  id<MTLCommandQueue> native;
};

struct _CpsrCommandBuffer {
  id<MTLCommandBuffer> native;
  
#ifndef NDEBUG
  const CpsrCommandQueue *commandQueue;
#endif
};

struct _CpsrGraphicsPipelineState {
  const CpsrDevice *device;
  MTLRenderPipelineDescriptor *desc;
  CpsrRasterizerDescriptor rasterizerDesc;
  id<MTLRenderPipelineState> native;
};

id<MTLRenderPipelineState> CpsrGraphicsPipelineStateGetNative(CpsrGraphicsPipelineState *pipelineState);

struct _CpsrGraphicsContext {
  const CpsrCommandBuffer     *commandBuffer;
  MTLRenderPassDescriptor     *desc;
  
  MTLIndexType                indexType;
  const CpsrBuffer            *indexBuffer;
  uint32_t                    indexBufferOffset;
  
  MTLPrimitiveType            primitiveTopology;
  id<MTLRenderCommandEncoder> native;
};

struct _CpsrComputePipelineState {
  const CpsrDevice *device;
  const CpsrShaderFunction *function;
  id<MTLComputePipelineState> native;
};

id<MTLComputePipelineState> CpsrComputePipelineStateGetNative(CpsrComputePipelineState *pipelineState);

struct _CpsrComputeContext {
  const CpsrCommandBuffer      *commandBuffer;
  id<MTLComputeCommandEncoder> native;
};

#endif // _CPSR_GRAPHICS_PRIVATE_H
