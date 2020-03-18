#include "CpsrGraphics+Private.h"

static inline size_t AlignUp(size_t inSize, size_t align) {
  assert(((align - 1) & align) == 0);
  
  const size_t alignmentMask = align - 1;
  return ((inSize + alignmentMask) & (~alignmentMask));
}

static inline MTLHeapDescriptor *CpsrHeapInitNativeDescriptor(const CpsrDevice *device, CpsrHeapType heapType) {
  MTLHeapDescriptor *heapDesc = [[MTLHeapDescriptor alloc] init];
  if (@available(iOS 13.0, macOS 10.15, *)) {
    heapDesc.resourceOptions = HeapTypeAsMetalType(device, heapType);
  } else {
    MTLResourceOptions options = HeapTypeAsMetalType(device, heapType);
    heapDesc.storageMode  = (options & MTLResourceStorageModeMask) >> MTLResourceStorageModeShift;
    heapDesc.cpuCacheMode = (options & MTLResourceCPUCacheModeMask) >> MTLResourceCPUCacheModeShift;
  }
  return heapDesc;
}

static inline CpsrHeap *CpsrHeapCreateFromNativeDescriptor(const CpsrDevice *device, CpsrHeapType heapType, MTLHeapDescriptor *desc) {
  id<MTLHeap> native = [device->native newHeapWithDescriptor:desc];
  [desc release];
  if (!native) {
    return NULL;
  }
  
  CpsrHeap *heap = CpsrAlloc(CpsrHeap);
  if (heap) {
    heap->device = device;
    heap->native = native;
    heap->heapType = heapType;
  }
  return heap;
}

CpsrHeap *CpsrHeapCreate(const CpsrDevice *device, size_t size, CpsrHeapType heapType) {
  CPSR_ASSUME(device);
  
  MTLHeapDescriptor *heapDesc = CpsrHeapInitNativeDescriptor(device, heapType);
  heapDesc.size = size;
  return CpsrHeapCreateFromNativeDescriptor(device, heapType, heapDesc);
}

CpsrHeap *CpsrHeapCreateFormTexture2DDescriptor(const CpsrDevice *device, const CpsrTexture2DDescriptor *descriptor, uint8_t count, CpsrHeapType heapType) {
  CPSR_ASSUME(device);
  CPSR_ASSUME(descriptor);
  
  MTLHeapDescriptor *heapDesc = CpsrHeapInitNativeDescriptor(device, heapType);
  MTLTextureDescriptor *nativeDesc = [[MTLTextureDescriptor alloc] init];
  Texture2DDescriptorToMetalType(descriptor, nativeDesc);
  if (@available(iOS 13.0, macOS 10.15, *)) {
    nativeDesc.resourceOptions = heapDesc.resourceOptions;
  } else {
    nativeDesc.storageMode = heapDesc.storageMode;
    nativeDesc.cpuCacheMode = heapDesc.cpuCacheMode;
  }
  
  MTLSizeAndAlign sizeAndAlign = [device->native heapTextureSizeAndAlignWithDescriptor:nativeDesc];
  heapDesc.size = count * AlignUp(sizeAndAlign.size, sizeAndAlign.align);
  
  [nativeDesc release];
  return CpsrHeapCreateFromNativeDescriptor(device, heapType, heapDesc);
}

CpsrHeap *CpsrHeapCreateFormTexture2DDescriptors(const CpsrDevice *device, const CpsrTexture2DDescriptor *descriptors, uint8_t maxCount, CpsrHeapType heapType) {
  CPSR_ASSUME(device);
  CPSR_ASSUME(descriptors);
  
  MTLHeapDescriptor *heapDesc = CpsrHeapInitNativeDescriptor(device, heapType);
  MTLTextureDescriptor *nativeDesc = [[MTLTextureDescriptor alloc] init];
  if (@available(iOS 13.0, macOS 10.15, *)) {
    nativeDesc.resourceOptions = heapDesc.resourceOptions;
  } else {
    nativeDesc.storageMode = heapDesc.storageMode;
    nativeDesc.cpuCacheMode = heapDesc.cpuCacheMode;
  }
  for (size_t i = 0; i < maxCount; ++i) {
    Texture2DDescriptorToMetalType(descriptors + i, nativeDesc);
    
    MTLSizeAndAlign sizeAndAlign = [device->native heapTextureSizeAndAlignWithDescriptor:nativeDesc];
    heapDesc.size += AlignUp(sizeAndAlign.size, sizeAndAlign.align);
  }
  [nativeDesc release];
  return CpsrHeapCreateFromNativeDescriptor(device, heapType, heapDesc);
}

void CpsrHeapDestroy(CpsrHeap *heap) {
  CPSR_ASSUME(heap);
  assert(heap->native.retainCount == 1);
  
  [heap->native release];
  CpsrDealloc(heap);
}
