#include "SnlfCore+Private.h"
#include "SnlfGraphicsFrame+Private.h"

#include "containers/SnlfLockFreeQueue.h"

// ---
// Data types
// ---
struct _SnlfGraphicsFrameAllocator {
  const CpsrDevice *device;
  CpsrHeapType heapType;
  SnlfGraphicsFrameFormat format;
  CpsrPixelFormat nativeFormat;
  CpsrSizeU32 size;
  SnlfLockFreeQueue frameQueue;
};

typedef struct {
  SnlfGraphicsFrameAllocatorRef allocator;
  osutil_atomic_int32_t refCount;
  CpsrHeap *heap;
} SnlfGraphicsFrameGPUHeapData;

// ---
// Prototypes
// ---
extern int32_t SnlfGraphicsFrameGpuHeapRelease(SnlfGraphicsFrameGPUHeapData *heapData);

// ---
// Frame
// ---
SnlfRGBSDRGraphicsFrame *SnlfRGBSDRGraphicsFrameCreate(SnlfGraphicsFrameHeapData *heapData, const CpsrTexture2D *texture) {
  SnlfRGBSDRGraphicsFrame *graphicsFrame = SnlfAlloc(SnlfRGBSDRGraphicsFrame);
  if (!graphicsFrame) {
    SnlfOutOfMemoryError();
    return NULL;
  }
  
  graphicsFrame->heapData = heapData;
  osutil_atomic_store32(&graphicsFrame->refCount, 1);
  graphicsFrame->timestamp = 0;
  graphicsFrame->frame = texture;
  return graphicsFrame;
}

bool SnlfGraphicsFrameDestroy(SnlfGraphicsFrameHeader *graphicsFrame) {
  assert(graphicsFrame);
  
  CpsrTexture2DDestroy(graphicsFrame->frame);
  SnlfDealloc(graphicsFrame);
  return false;
}

int32_t SnlfGraphicsFrameAddRef(SnlfGraphicsFrameHeader *graphicsFrame) {
  int32_t ret = osutil_atomic_fetch_increment32(&graphicsFrame->refCount);
#ifdef DEBUG_REFERENCE_COUNT
  SnlfVerboseLogFormat("Add reference count: %d on %p (SnlfGraphicsFrame)", ret + 1, obj);
#endif
  return ++ret;
}

int32_t SnlfGraphicsFrameRelease(SnlfGraphicsFrameHeader *graphicsFrame) {
  int32_t ret = osutil_atomic_fetch_decrement32(&graphicsFrame->refCount);
#ifdef DEBUG_REFERENCE_COUNT
  SnlfVerboseLogFormat("Release reference count: %d on %p (SnlfGraphicsFrame)", ret - 1, obj);
#endif
  if (ret == 1) {
    SnlfGraphicsFrameAllocatorRef allocator = graphicsFrame->heapData->allocator;
    if (CpsrSizeU32Equal(allocator->size, CpsrTexture2DGetSize(graphicsFrame->frame))) {
      SnlfLockFreeQueueEnqueue(&allocator->frameQueue, (intptr_t)graphicsFrame);
    } else {
      SnlfGraphicsFrameDestroy(graphicsFrame);
      SnlfGraphicsFrameGpuHeapRelease((SnlfGraphicsFrameGPUHeapData *)graphicsFrame->heapData);
    }
  }
  return --ret;
}

int32_t SnlfGraphicsFrameGetRefCount(SnlfGraphicsFrameHeader *graphicsFrame) {
  int32_t ret = osutil_atomic_load32(&graphicsFrame->refCount);
  return ret;
}

void SnlfRGBSDRGraphicsFrameWrite(SnlfRGBSDRGraphicsFrame *graphicsFrame, intptr_t data, size_t bytesPerRow) {
  assert(graphicsFrame);
  assert(graphicsFrame->heapData->allocator->heapType == CPSR_HEAP_TYPE_UPLOAD);
  
  const CpsrTexture2D *texture = graphicsFrame->frame;
  CpsrTexture2DWrite(texture, (void *)data, bytesPerRow);
}

// ---
// Heap
// ---
int32_t SnlfGraphicsFrameHeapAddRef(SnlfGraphicsFrameHeapData *heapData) {
  int32_t ret = osutil_atomic_fetch_increment32(&heapData->refCount);
#ifdef DEBUG_REFERENCE_COUNT
  SnlfVerboseLogFormat("Add reference count: %d on %p (SnlfGraphicsFrameHeap)", ret + 1, obj);
#endif
  return ++ret;
}

int32_t SnlfGraphicsFrameHeapGetRefCount(SnlfGraphicsFrameHeapData *heapData) {
  int32_t ret = osutil_atomic_load32(&heapData->refCount);
  return ret;
}

// ---
// GPU Heap
// ---
SnlfGraphicsFrameGPUHeapData *SnlfGraphicsFrameGpuHeapCreate(SnlfGraphicsFrameAllocatorRef allocator, size_t count) {
  SnlfGraphicsFrameGPUHeapData *gpuHeap = SnlfAlloc(SnlfGraphicsFrameGPUHeapData);
  if (!gpuHeap) {
    SnlfOutOfMemoryError();
    return NULL;
  }
  
  CpsrTexture2DDescriptor textureDesc;
  textureDesc.size = allocator->size;
  textureDesc.arrayLength = 1;
  textureDesc.pixelFormat = allocator->nativeFormat;
  textureDesc.usage = CPSR_TEXTURE_USAGE_READ;
  
  CpsrHeap *heap = CpsrHeapCreateFormTexture2DDescriptor(allocator->device, &textureDesc, count, allocator->heapType);
  if (!heap) {
    SnlfDealloc(gpuHeap);
    return NULL;
  }
  
  gpuHeap->allocator = allocator;
  osutil_atomic_store32(&gpuHeap->refCount, 1 + count);
  gpuHeap->heap = heap;
  
  for (size_t i = 0; i < count; ++i) {
    CpsrTexture2D *texture = CpsrTexture2DCreateFromHeap(heap, &textureDesc);
    SnlfRGBSDRGraphicsFrame *frame = SnlfRGBSDRGraphicsFrameCreate((SnlfGraphicsFrameHeapData *)gpuHeap, texture);
    SnlfLockFreeQueueEnqueue(&allocator->frameQueue, (intptr_t)frame);
  }
  return gpuHeap;
}

int32_t SnlfGraphicsFrameGpuHeapRelease(SnlfGraphicsFrameGPUHeapData *heapData) {
  assert(heapData);
  
  int32_t ret = osutil_atomic_fetch_decrement32(&heapData->refCount);
#ifdef DEBUG_REFERENCE_COUNT
  SnlfVerboseLogFormat("Release reference count: %d on %p (SnlfGraphicsFrameGpuHeap)", ret - 1, obj);
#endif
  if (ret == 1) {
    CpsrHeapDestroy(heapData->heap);
    SnlfDealloc(heapData);
  }
  return --ret;
}

// ---
// Allocator
// ---
void SnlfGraphicsFrameAllocatorInitPrivate(SnlfGraphicsFrameAllocatorRef allocator, const CpsrDevice *device, CpsrHeapType heapType, SnlfGraphicsFrameFormat format, CpsrSizeU32 size) {
  allocator->device = device;
  allocator->heapType = heapType;
  allocator->format = format;
  allocator->size = size;
  SnlfLockFreeQueueInit(&allocator->frameQueue);
}

SnlfGraphicsFrameAllocatorRef SnlfGraphicsFrameAllocatorInit(const CpsrDevice *device, CpsrHeapType heapType, SnlfGraphicsFrameFormat format, CpsrSizeU32 size) {
  SnlfGraphicsFrameAllocatorRef allocator = SnlfAllocRef(SnlfGraphicsFrameAllocator);
  if (!allocator) {
    SnlfOutOfMemoryError();
    return NULL;
  }
  
  SnlfGraphicsFrameAllocatorInitPrivate(allocator, device, heapType, format, size);
  return allocator;
}

void SnlfGraphicsFrameAllocatorUninit(SnlfGraphicsFrameAllocatorRef allocator) {
  SnlfLockFreeQueue *frameQueue = &allocator->frameQueue;
  
  SnlfGraphicsFrameHeader *frame = NULL;
  while ((frame = (SnlfGraphicsFrameHeader *)SnlfLockFreeQueueDequeue(frameQueue))) {
    SnlfGraphicsFrameRelease(frame);
  }
  SnlfLockFreeQueueUninit(frameQueue);
}

SnlfGraphicsFrameFormat SnlfGraphicsFrameAllocatorGetFormat(SnlfGraphicsFrameAllocatorRef allocator) {
  return allocator->format;
}

bool SnlfGraphicsFrameAllocatorSetFormat(SnlfGraphicsFrameAllocatorRef allocator, SnlfGraphicsFrameFormat format) {
  allocator->format = format;
  
  // 4-bit RGB
  if (format == SNLF_GRAPHICS_FRAME_RGBX4
      || format == SNLF_GRAPHICS_FRAME_RGBA4
      || format == SNLF_GRAPHICS_FRAME_XBGR4
      || format == SNLF_GRAPHICS_FRAME_ABGR4) {
    CpsrPixelFormat nativeFormat = CPSR_PIXELFORMAT_ABGR4_UNORM;
    CpsrPixelFormatCapabilities capabilities = CpsrDeviceGetPixelFormatCapabilities(allocator->device, nativeFormat);
    if (capabilities.load) {
      allocator->nativeFormat = nativeFormat;
    } else {
      nativeFormat = CPSR_PIXELFORMAT_RG8_UINT;
      capabilities = CpsrDeviceGetPixelFormatCapabilities(allocator->device, nativeFormat);
      if (capabilities.load) {
        allocator->nativeFormat = nativeFormat;
      } else {
        SnlfPixelFormatError();
        return false;
      }
    }
    return true;
  }
  
  // 8-bit RGB
  if (format == SNLF_GRAPHICS_FRAME_RGBX8
      || format == SNLF_GRAPHICS_FRAME_RGBA8) {
    CpsrPixelFormat nativeFormat = CPSR_PIXELFORMAT_RGBA8_UNORM;
    CpsrPixelFormatCapabilities capabilities = CpsrDeviceGetPixelFormatCapabilities(allocator->device, nativeFormat);
    if (capabilities.load) {
      allocator->nativeFormat = nativeFormat;
    } else {
      SnlfPixelFormatError();
      return false;
    }
    return true;
  }
  if (format == SNLF_GRAPHICS_FRAME_BGRX8
      || format == SNLF_GRAPHICS_FRAME_BGRA8) {
    CpsrPixelFormat nativeFormat = CPSR_PIXELFORMAT_BGRA8_UNORM;
    CpsrPixelFormatCapabilities capabilities = CpsrDeviceGetPixelFormatCapabilities(allocator->device, nativeFormat);
    if (capabilities.load) {
      allocator->nativeFormat = nativeFormat;
    } else {
      nativeFormat = CPSR_PIXELFORMAT_RGBA8_UNORM;
      capabilities = CpsrDeviceGetPixelFormatCapabilities(allocator->device, nativeFormat);
      if (capabilities.load) {
        allocator->nativeFormat = nativeFormat;
      } else {
        SnlfPixelFormatError();
        return false;
      }
    }
    return true;
  }
  
  return false;
}


CpsrSizeU32 SnlfGraphicsFrameAllocatorGetSize(SnlfGraphicsFrameAllocatorRef allocator) {
  return allocator->size;
}

void SnlfGraphicsFrameAllocatorSetSize(SnlfGraphicsFrameAllocatorRef allocator, CpsrSizeU32 size) {
  allocator->size = size;
}
