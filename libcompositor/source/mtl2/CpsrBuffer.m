#include "CpsrGraphics+Private.h"

static inline CpsrBuffer *CreateBuffer(const CpsrDevice *device, id<MTLBuffer> native, CpsrHeapType heapType, CpsrBufferType bufferType) {
  CpsrBuffer *buffer = CpsrAlloc(CpsrBuffer);
  if (buffer) {
    buffer->native = native;
    
#ifndef NDEBUG
    buffer->device = device;
    buffer->heapType = heapType;
    buffer->bufferType = bufferType;
#endif
  }
  return buffer;
}

CpsrBuffer *CpsrBufferCreateFromSize(const CpsrDevice *device, size_t sizeInBytes, CpsrHeapType heapType, CpsrBufferType bufferType) {
  CPSR_ASSUME(device);
  
  MTLResourceOptions options = HeapTypeAsMetalType(device, heapType);
  id<MTLBuffer> native = [device->native newBufferWithLength:sizeInBytes options:options];
  if (!native) {
    return NULL;
  }
  
  CpsrBuffer *buffer = CreateBuffer(device, native, heapType, bufferType);
  return buffer;
}

CpsrBuffer *CpsrBufferCreateFromData(const CpsrDevice *device, const void *data, size_t sizeInBytes, CpsrBufferType bufferType) {
  CPSR_ASSUME(device);
  CPSR_ASSUME(data);
  
  MTLResourceOptions options = HeapTypeAsMetalType(device, CPSR_HEAP_TYPE_UPLOAD);
  id<MTLBuffer> native = [device->native newBufferWithBytes:data length:sizeInBytes options:options];
  if (!native) {
    return NULL;
  }
  
  CpsrBuffer *buffer = CreateBuffer(device, native, CPSR_HEAP_TYPE_UPLOAD, bufferType);
  return buffer;
}

void CpsrBufferDestroy(CpsrBuffer *buffer) {
  CPSR_ASSUME(buffer);
  
  [buffer->native setPurgeableState:MTLPurgeableStateEmpty];
  [buffer->native release];
  CpsrDealloc(buffer);
}

// ---
// Get property
// ---
size_t CpsrBufferGetSize(const CpsrBuffer *buffer) {
  CPSR_ASSUME(buffer);
  
  return buffer->native.length;
}

size_t CpsrBufferGetCapacity(const CpsrBuffer *buffer) {
  CPSR_ASSUME(buffer);
  
  return buffer->native.allocatedSize;
}

// ---
// Set property
// ---
void CpsrBufferSetName(const CpsrBuffer *buffer, const char *name, size_t maxCount) {
  CPSR_ASSUME(buffer);
  CPSR_ASSUME(name);
  
  NSString *nsNameString = [[NSString alloc] initWithBytesNoCopy:(void *)name length:maxCount encoding:NSUTF8StringEncoding freeWhenDone:NO];
  buffer->native.label = nsNameString;
  [nsNameString release];
}

// ---
// Map/Unmap function
// ---
bool CpsrBufferMap(const CpsrBuffer *buffer, void **data) {
  CPSR_ASSUME(buffer);
  CPSR_ASSUME(buffer->heapType & CPSR_HEAP_TYPE_UPLOAD || buffer->heapType & CPSR_HEAP_TYPE_READBACK);
  CPSR_ASSUME(data);
  
  *data = buffer->native.contents;
  return false;
}

void CpsrBufferUnmap(const CpsrBuffer *buffer, CpsrRange range) {
  CPSR_ASSUME(buffer);
  CPSR_ASSUME(buffer->heapType & CPSR_HEAP_TYPE_UPLOAD || buffer->heapType & CPSR_HEAP_TYPE_READBACK);
  assert(buffer->native.length >= range.location + range.length);
  
#if TARGET_OS_OSX || TARGET_OS_MACCATALYST
  if (buffer->native.storageMode == MTLStorageModeManaged) {
    NSRange nativeRange = NSMakeRange(range.location, range.length);
    [buffer->native didModifyRange:nativeRange];
  }
#endif
}

// ---
// Read function
// ---
bool CpsrBufferRead(const CpsrBuffer *buffer, void *data) {
  CPSR_ASSUME(buffer);
  CPSR_ASSUME(buffer->heapType & CPSR_HEAP_TYPE_READBACK);
  CPSR_ASSUME(data);
  
  size_t size = buffer->native.length;
  memcpy(data, buffer->native.contents, size);
  return false;
}

// ---
// Write function
// ---
bool CpsrBufferWrite(CpsrBuffer *buffer, const void *data) {
  CPSR_ASSUME(buffer);
  CPSR_ASSUME(buffer->heapType & CPSR_HEAP_TYPE_UPLOAD);
  CPSR_ASSUME(data);
  
  size_t size = buffer->native.length;
  memcpy(buffer->native.contents, data, size);
  
#if TARGET_OS_OSX || TARGET_OS_MACCATALYST
  if (buffer->native.storageMode == MTLStorageModeManaged) {
    NSRange range = NSMakeRange(0, size);
    [buffer->native didModifyRange:range];
  }
#endif
  return false;
}
