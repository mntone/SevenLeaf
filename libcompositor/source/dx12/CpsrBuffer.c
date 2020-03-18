#include "CpsrGraphics+Private.h"

static inline CpsrBuffer *CreateBuffer(const CpsrDevice *device,
                                       size_t size,
                                       CpsrHeapType heapType,
                                       CpsrBufferType bufferType) {
  size_t capacity = max(size, 16);

  D3D12_HEAP_PROPERTIES prop;
  if (heapType & CPSR_HEAP_TYPE_UPLOAD) {
    prop.Type = D3D12_HEAP_TYPE_UPLOAD;
  } else if (heapType & CPSR_HEAP_TYPE_READBACK) {
    prop.Type = D3D12_HEAP_TYPE_READBACK;
  } else {
    prop.Type = D3D12_HEAP_TYPE_DEFAULT;
  }
  prop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
  prop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
  prop.CreationNodeMask = 0;
  prop.VisibleNodeMask = 0;

  D3D12_RESOURCE_DESC desc;
  desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
  desc.Alignment = 0;
  desc.Width = capacity;
  desc.Height = 1;
  desc.DepthOrArraySize = 1;
  desc.MipLevels = 1;
  desc.Format = DXGI_FORMAT_UNKNOWN;
  desc.SampleDesc.Count = 1;
  desc.SampleDesc.Quality = 0;
  desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
  desc.Flags = D3D12_RESOURCE_FLAG_NONE;

  D3D12_RESOURCE_STATES states;
  if (bufferType == CPSR_INDEX_BUFFER) {
    states = D3D12_RESOURCE_STATE_INDEX_BUFFER | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
  } else {
    states = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
  }

  ID3D12Resource *resource;
  HRESULT hr = ID3D12Device_CreateCommittedResource(device->nativeDevice,
                                                    &prop,
                                                    D3D12_HEAP_FLAG_ALLOW_ONLY_BUFFERS,
                                                    &desc,
                                                    states,
                                                    NULL,
                                                    &IID_ID3D12Resource,
                                                    &resource);
  if (FAILED(hr)) {
    return NULL;
  }

  CpsrBuffer *buffer = CpsrAlloc(CpsrBuffer);
  if (buffer) {
    buffer->device = device;
    buffer->native = resource;
    buffer->size = size;

#ifndef NDEBUG
    buffer->heapType = heapType;
    buffer->bufferType = bufferType;
#endif
  }
  return buffer;
}

CpsrBuffer *CpsrBufferCreateFromSize(const CpsrDevice *device,
                                     size_t sizeInBytes,
                                     CpsrHeapType heapType,
                                     CpsrBufferType bufferType) {
  CPSR_ASSUME(device);

  CpsrBuffer *buffer = CreateBuffer(device, sizeInBytes, heapType, bufferType);
  return buffer;
}

CpsrBuffer *CpsrBufferCreateFromData(const CpsrDevice *device,
                                     const void *data,
                                     size_t sizeInBytes,
                                     CpsrBufferType bufferType) {
  CPSR_ASSUME(device);
  CPSR_ASSUME(data);

  CpsrBuffer *buffer = CreateBuffer(device, sizeInBytes, true, bufferType);
  if (buffer) {
    CpsrBufferWrite(buffer, data);
  }
  return buffer;
}

void CpsrBufferDestroy(CpsrBuffer *buffer) {
  CPSR_ASSUME(buffer);

  ID3D12Resource_Release(buffer->native);
  CpsrDealloc(buffer);
}

// ---
// Get property
// ---
size_t CpsrBufferGetSize(const CpsrBuffer *buffer) {
  CPSR_ASSUME(buffer);

  return buffer->size;
}

size_t CpsrBufferGetCapacity(const CpsrBuffer *buffer) {
  CPSR_ASSUME(buffer);

  D3D12_RESOURCE_DESC desc = ID3D12Resource_GetDesc(buffer->native);
  return desc.Width;
}

// ---
// Set property
// ---
void CpsrBufferSetName(const CpsrBuffer *buffer, const char *name, size_t maxCount) {
  CPSR_ASSUME(buffer);
  CPSR_ASSUME(name);

  wchar_t *utf16Name = CpsrGetUtf16String(name, (int)maxCount, NULL);
  ID3D12Resource_SetName(buffer->native, utf16Name);
  free(utf16Name);
}

// ---
// Map/Unmap function
// ---
bool CpsrBufferMap(const CpsrBuffer *buffer, void **data) {
  CPSR_ASSUME(buffer);
  CPSR_ASSUME(buffer->heapType & CPSR_HEAP_TYPE_UPLOAD || buffer->heapType & CPSR_HEAP_TYPE_READBACK);
  CPSR_ASSUME(data);

  HRESULT hr = ID3D12Resource_Map(buffer->native, 0, NULL, data);
  if (FAILED(hr)) {
    return true;
  }
  return false;
}

void CpsrBufferUnmap(const CpsrBuffer *buffer, CpsrRange range) {
  CPSR_ASSUME(buffer);
  CPSR_ASSUME(buffer->heapType & CPSR_HEAP_TYPE_UPLOAD || buffer->heapType & CPSR_HEAP_TYPE_READBACK);
  CPSR_ASSUME(buffer->size >= range.location + range.length);

  D3D12_RANGE nativeRange = { range.location, range.location + range.length };
  ID3D12Resource_Unmap(buffer->native, 0, &nativeRange);
}

// ---
// Read function
// ---
bool CpsrBufferRead(const CpsrBuffer *buffer, void *data) {
  CPSR_ASSUME(buffer);
  CPSR_ASSUME(buffer->heapType & CPSR_HEAP_TYPE_READBACK);
  CPSR_ASSUME(data);

  uint8_t *src = NULL;
  HRESULT hr = ID3D12Resource_Map(buffer->native, 0, NULL, (void **)&src);
  if (FAILED(hr)) {
    return true;
  }

  size_t size = buffer->size;
  memcpy(data, src, size);

  D3D12_RANGE range = { 0, 0 };
  ID3D12Resource_Unmap(buffer->native, 0, &range);
  return false;
}

// ---
// Write function
// ---
bool CpsrBufferWrite(CpsrBuffer *buffer, const void *data) {
  CPSR_ASSUME(buffer);
  CPSR_ASSUME(buffer->heapType & CPSR_HEAP_TYPE_UPLOAD);
  CPSR_ASSUME(data);

  uint8_t *dst = NULL;
  HRESULT hr = ID3D12Resource_Map(buffer->native, 0, NULL, (void **)&dst);
  if (FAILED(hr)) {
    return true;
  }

  size_t size = buffer->size;
  memcpy(dst, data, size);

  ID3D12Resource_Unmap(buffer->native, 0, NULL);
  return false;
}
