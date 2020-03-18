#include "CpsrGraphics+Private.h"

CpsrTexture2D *CpsrTexture2DCreate(const CpsrDevice *cpsrDevice,
                                   const CpsrTexture2DDescriptor *desc,
                                   CpsrHeapType heapType) {
  CPSR_ASSUME(cpsrDevice);
  CPSR_ASSUME(desc);

  ID3D12Device *device;
  device = cpsrDevice->nativeDevice;
  if (!device) {
    return NULL;
  }
  ID3D12Device_AddRef(device);

  D3D12_HEAP_PROPERTIES prop;
  if (heapType & CPSR_HEAP_TYPE_UPLOAD) {
    prop.Type = D3D12_HEAP_TYPE_UPLOAD;
  } else {
    prop.Type = D3D12_HEAP_TYPE_DEFAULT;
  }
  prop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
  prop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
  prop.CreationNodeMask = 0;
  prop.VisibleNodeMask = 0;

  D3D12_RESOURCE_DESC resourceDesc;
  resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
  resourceDesc.Alignment;
  resourceDesc.Width = desc->size.width;
  resourceDesc.Height = desc->size.height;
  resourceDesc.DepthOrArraySize = desc->arrayLength;
  resourceDesc.MipLevels = 1;
  resourceDesc.Format = PixelFormatAsDx12Type(desc->pixelFormat);
  resourceDesc.SampleDesc.Count = 1;
  resourceDesc.SampleDesc.Quality = 0;
  if (heapType & CPSR_HEAP_TYPE_UPLOAD || heapType & CPSR_HEAP_TYPE_READBACK) {
    resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
  } else {
    resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
  }
  resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
  if (desc->usage & CPSR_TEXTURE_USAGE_WRITE) {
    resourceDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
  }
  if (desc->usage & CPSR_TEXTURE_USAGE_RENDER_TARGET) {
    resourceDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
  }

  ID3D12Resource *resource;
  HRESULT hr = ID3D12Device_CreateCommittedResource(device,
                                                    &prop,
                                                    D3D12_HEAP_FLAG_ALLOW_ONLY_NON_RT_DS_TEXTURES,
                                                    &resourceDesc,
                                                    D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
                                                    NULL,
                                                    &IID_ID3D12Resource,
                                                    &resource);
  if (FAILED(hr)) {
    ID3D12Device_Release(device);
    return NULL;
  }

  D3D12_DESCRIPTOR_HEAP_DESC heapDesc;
  if (desc->usage & CPSR_TEXTURE_USAGE_RENDER_TARGET) {
    heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
  } else {
    heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
  }
  heapDesc.NumDescriptors = 1;
  if (desc->usage & CPSR_TEXTURE_USAGE_READ) {
    heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
  } else {
    heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
  }
  heapDesc.NodeMask = 0;

  ID3D12DescriptorHeap *heap = NULL;
  if (desc->usage & CPSR_TEXTURE_USAGE_RENDER_TARGET) {
    hr = ID3D12Device_CreateDescriptorHeap(device, &heapDesc, &IID_ID3D12DescriptorHeap, &heap);
    if (FAILED(hr)) {
      ID3D12Device_Release(device);
      return NULL;
    }

    D3D12_RENDER_TARGET_VIEW_DESC rtvDesc;
    rtvDesc.Format = PixelFormatAsDx12Type(desc->pixelFormat);
    rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
    if (desc->arrayLength > 1) {
      rtvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
      rtvDesc.Texture2DArray.MipSlice = 1;
      rtvDesc.Texture2DArray.FirstArraySlice = 0;
      rtvDesc.Texture2DArray.ArraySize = desc->arrayLength;
      rtvDesc.Texture2DArray.PlaneSlice = 0;
    } else {
      rtvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
      rtvDesc.Texture2D.MipSlice = 1;
      rtvDesc.Texture2D.PlaneSlice = 0;
    }

    D3D12_CPU_DESCRIPTOR_HANDLE cpuDesc = ID3D12DescriptorHeap_GetCPUDescriptorHandleForHeapStart(heap);
    ID3D12Device_CreateRenderTargetView(device, resource, &rtvDesc, cpuDesc);
  }

  CpsrTexture2D *texture = CpsrAlloc(CpsrTexture2D);
  if (texture) {
    texture->device = cpsrDevice;
    texture->nativeResource = resource;
    texture->nativeHeap = heap;

#ifndef NDEUG
    texture->heapType = heapType;
    texture->usage = desc->usage;
#endif
  }
  return texture;
}

void CpsrTexture2DDestroy(CpsrTexture2D *texture2D) {
  CPSR_ASSUME(texture2D);

  ID3D12DescriptorHeap_Release(texture2D->nativeHeap);
  ID3D12Resource_Release(texture2D->nativeResource);
  CpsrDealloc(texture2D);
}

CpsrSizeU32 CpsrTexture2DGetSize(const CpsrTexture2D *texture2D) {
  CPSR_ASSUME(texture2D);

  D3D12_RESOURCE_DESC desc = ID3D12Resource_GetDesc(texture2D->nativeResource);
  CpsrSizeU32 size = {
      (uint32_t)desc.Width,
      (uint32_t)desc.Height,
  };
  return size;
}

size_t CpsrTexture2DGetLength(const CpsrTexture2D *texture2D) {
  CPSR_ASSUME(texture2D);

  D3D12_RESOURCE_DESC desc = ID3D12Resource_GetDesc(texture2D->nativeResource);
  return desc.DepthOrArraySize;
}

void CpsrTexture2DSetName(const CpsrTexture2D *texture2D, const char *name, size_t maxCount) {
  CPSR_ASSUME(texture2D);
  CPSR_ASSUME(name);

  wchar_t *utf16Name = CpsrGetUtf16String(name, (int)maxCount, NULL);
  ID3D12Resource_SetName(texture2D->nativeResource, utf16Name);
  free(utf16Name);
}

// ---
// Internal functions
// ---
size_t CpsrTexture2DGetNativeHandle(const CpsrTexture2D *texture2D) {
  CPSR_ASSUME(texture2D);
  CPSR_ASSUME(texture2D->usage & CPSR_TEXTURE_USAGE_RENDER_TARGET);

  D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle =
      ID3D12DescriptorHeap_GetCPUDescriptorHandleForHeapStart(texture2D->nativeHeap);
  return cpuHandle.ptr;
}
