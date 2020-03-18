#include "CpsrGraphics+Private.h"

#include <IOSurface/IOSurface.h>

static inline CpsrTexture2D *CpsrTexture2DCreateFromNative(const CpsrDevice *device, id<MTLTexture> native, CpsrHeapType heapType, CpsrTextureUsage usage) {
  if (!native) {
    return NULL;
  }
  
  CpsrTexture2D *texture = CpsrAlloc(CpsrTexture2D);
  if (texture) {
    texture->native = native;
    
#ifndef NDEBUG
    texture->device = device;
    texture->heapType = heapType;
    texture->usage = usage;
#endif
  }
  return texture;
}

CpsrTexture2D *CpsrTexture2DCreate(const CpsrDevice *device, const CpsrTexture2DDescriptor *desc, CpsrHeapType heapType) {
  CPSR_ASSUME(device);
  CPSR_ASSUME(desc);
  
  MTLTextureDescriptor *textureDesc = Texture2DDescriptorAsMetalType(desc);
  textureDesc.resourceOptions = HeapTypeAsMetalType(device, heapType);
  if (heapType & CPSR_HEAP_TYPE_READBACK) {
    if (@available(iOS 12.0, macOS 10.14, *)) {
      textureDesc.allowGPUOptimizedContents = NO;
    }
  }
  
  id<MTLTexture> native = [device->native newTextureWithDescriptor:textureDesc];
  [textureDesc release];
  return CpsrTexture2DCreateFromNative(device, native, heapType, desc->usage);
}

CpsrTexture2D *CpsrTexture2DCreateFromHeap(const CpsrHeap *heap,
                                           const CpsrTexture2DDescriptor *desc) {
  CPSR_ASSUME(heap);
  CPSR_ASSUME(desc);
  
  MTLTextureDescriptor *textureDesc = Texture2DDescriptorAsMetalType(desc);
  if (@available(iOS 13.0, macOS 10.15, *)) {
    textureDesc.resourceOptions = heap->native.resourceOptions;
  } else {
    textureDesc.storageMode = heap->native.storageMode;
    textureDesc.cpuCacheMode = heap->native.cpuCacheMode;
  }
  if ((heap->device->hasUnifiedMemory && heap->native.resourceOptions == (MTLResourceStorageModeShared | MTLResourceCPUCacheModeWriteCombined))
      || heap->native.resourceOptions == (MTLResourceStorageModeManaged | MTLResourceCPUCacheModeWriteCombined)) {
    if (@available(iOS 12.0, macOS 10.14, *)) {
      textureDesc.allowGPUOptimizedContents = NO;
    }
  }
  
  id<MTLTexture> native = [heap->native newTextureWithDescriptor:textureDesc];
  [textureDesc release];
  return CpsrTexture2DCreateFromNative(heap->device, native, heap->heapType, desc->usage);
}

CpsrTexture2D *CpsrTexture2DCreateFromIOSurface(const CpsrDevice *device, IOSurfaceRef ioSurface, CpsrPixelFormat pixelFormat) {
  CPSR_ASSUME(device);
  CPSR_ASSUME(ioSurface);
  
  MTLPixelFormat nativePixelFormat = PixelFormatAsMetalType(pixelFormat);
  NSUInteger width = IOSurfaceGetWidth(ioSurface);
  NSUInteger height = IOSurfaceGetHeight(ioSurface);
  MTLTextureDescriptor *textureDesc = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:nativePixelFormat width:width height:height mipmapped:NO];
  textureDesc.storageMode = MTLStorageModePrivate;
    
  // default (macOS): MTLHazardTrackingModeDefault
  if (@available(iOS 13.0, macOS 10.15, *)) {
    textureDesc.hazardTrackingMode = MTLHazardTrackingModeUntracked;
  }
  
  id<MTLTexture> native = [device->native newTextureWithDescriptor:textureDesc iosurface:ioSurface plane:0];
  return CpsrTexture2DCreateFromNative(device, native, CPSR_HEAP_TYPE_DEFAULT, CPSR_TEXTURE_USAGE_READ);
}

CpsrTexture2D *CpsrTexture2DCreateFromNativeHandle(const CpsrDevice *device, id<MTLTexture> native) {
  return CpsrTexture2DCreateFromNative(device, native, CPSR_HEAP_TYPE_DEFAULT, 0);
}

void CpsrTexture2DDestroy(CpsrTexture2D *texture2D) {
  CPSR_ASSUME(texture2D);
  assert(texture2D->native.retainCount == 1);
  
  [texture2D->native release];
  CpsrDealloc(texture2D);
}

// ---
// Get property
// ---
CpsrSizeU32 CpsrTexture2DGetSize(const CpsrTexture2D *texture2D) {
  CPSR_ASSUME(texture2D);
  
  CpsrSizeU32 size = {
    (uint32_t)texture2D->native.width,
    (uint32_t)texture2D->native.height,
  };
  return size;
}

size_t CpsrTexture2DGetLength(const CpsrTexture2D *texture2D) {
  CPSR_ASSUME(texture2D);
  
  return texture2D->native.arrayLength;
}

// ---
// Write function
// ---
bool CpsrTexture2DWrite(const CpsrTexture2D *texture2D, const void *data, size_t bytesPerRow) {
  CPSR_ASSUME(texture2D);
  CPSR_ASSUME(texture2D->heapType & CPSR_HEAP_TYPE_UPLOAD);
  CPSR_ASSUME(data);
  
  MTLRegion region = MTLRegionMake2D(0, 0, texture2D->native.width, texture2D->native.height);
  [texture2D->native replaceRegion:region mipmapLevel:0 withBytes:data bytesPerRow:bytesPerRow];
  return false;
}
