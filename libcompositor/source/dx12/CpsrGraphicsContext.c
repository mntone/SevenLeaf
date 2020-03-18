#include "CpsrGraphics+Private.h"

// clang-format off
static inline CpsrPrimitiveTopology PrimitiveTopologyAsCpsrType(D3D12_PRIMITIVE_TOPOLOGY primitiveTopology) {
  switch (primitiveTopology) {
  case D3D_PRIMITIVE_TOPOLOGY_POINTLIST:     return CPSR_PRIMITIVE_TOPOLOGY_POINT;
  case D3D_PRIMITIVE_TOPOLOGY_LINELIST:      return CPSR_PRIMITIVE_TOPOLOGY_LINE;
  case D3D_PRIMITIVE_TOPOLOGY_LINESTRIP:     return CPSR_PRIMITIVE_TOPOLOGY_LINE_STRIP;
  case D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP: return CPSR_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
  case D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST:
  default:                                   return CPSR_PRIMITIVE_TOPOLOGY_TRIANGLE;
  }
}

static inline D3D12_PRIMITIVE_TOPOLOGY PrimitiveTopologyAsDx12Type(CpsrPrimitiveTopology primitiveTopology) {
  switch (primitiveTopology) {
  case CPSR_PRIMITIVE_TOPOLOGY_POINT:          return D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
  case CPSR_PRIMITIVE_TOPOLOGY_LINE:           return D3D_PRIMITIVE_TOPOLOGY_LINELIST;
  case CPSR_PRIMITIVE_TOPOLOGY_LINE_STRIP:     return D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;
  case CPSR_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP: return D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
  case CPSR_PRIMITIVE_TOPOLOGY_TRIANGLE:
  default:                                     return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
  }
}

static inline DXGI_FORMAT IndexTypeAsDx12Type(CpsrIndexType indexType) {
  switch (indexType) {
  case CPSR_INDEX_TYPE_UINT16: return DXGI_FORMAT_R16_UINT;
  case CPSR_INDEX_TYPE_UINT32:
  default:                     return DXGI_FORMAT_R32_UINT;
  }
}
// clang-format on

CpsrGraphicsContext *CpsrGraphicsContextCreate(const CpsrCommandBuffer *commandBuffer) {
  CPSR_ASSUME(commandBuffer);

  CpsrGraphicsContext *graphicsContext = CpsrAlloc(CpsrGraphicsContext);
  if (graphicsContext) {
    graphicsContext->commandBuffer = commandBuffer;
    graphicsContext->native = NULL;
  }
  return graphicsContext;
}

void CpsrGraphicsContextDestroy(CpsrGraphicsContext *graphicsContext) {
  CPSR_ASSUME(graphicsContext);

  if (graphicsContext->native) {
    ID3D12GraphicsCommandList_Release(graphicsContext->native);
  }
  CpsrDealloc(graphicsContext);
}

void CpsrGraphicsContextClearRenderTarget(CpsrGraphicsContext *graphicsContext, uint8_t index, CpsrClearColor color) {
  CPSR_ASSUME(graphicsContext);
  CPSR_ASSUME(!graphicsContext->native);
  CPSR_ASSUME(index < 8);

  ID3D12GraphicsCommandList_ClearRenderTargetView(graphicsContext->native, )
}

void CpsrGraphicsContextSetRenderTargetFromTexture2D(CpsrGraphicsContext *graphicsContext,
                                                     const CpsrTexture2D *renderTarget) {
  CPSR_ASSUME(graphicsContext);
  CPSR_ASSUME(!graphicsContext->native);
  CPSR_ASSUME(renderTarget);
  CPSR_ASSUME(renderTarget->usage & CPSR_TEXTURE_USAGE_RENDER_TARGET);

  D3D12_CPU_DESCRIPTOR_HANDLE renderTargetHandle;
  renderTargetHandle.ptr = CpsrTexture2DGetNativeHandle(renderTarget);
  ID3D12GraphicsCommandList_OMSetRenderTargets(graphicsContext->native, 1, &renderTargetHandle, FALSE, NULL);
}

void CpsrGraphicsContextSetRenderTargetFromSwapChain(CpsrGraphicsContext *graphicsContext, CpsrSwapChain *swapChain) {
  CPSR_ASSUME(graphicsContext);
  CPSR_ASSUME(!graphicsContext->native);
  CPSR_ASSUME(swapChain);

  D3D12_RESOURCE_BARRIER resourceBarrier;
  resourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
  resourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
  resourceBarrier.Transition.pResource = CpsrSwapChainGetNativeRenderTargetResource(swapChain);
  resourceBarrier.Transition.Subresource = 0;
  resourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COMMON;
  resourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
  ID3D12GraphicsCommandList_ResourceBarrier(graphicsContext->native, 1, &resourceBarrier);

  D3D12_CPU_DESCRIPTOR_HANDLE renderTargetHandle;
  renderTargetHandle.ptr = CpsrSwapChainGetNativeRenderTargetHandle(swapChain);
  ID3D12GraphicsCommandList_OMSetRenderTargets(graphicsContext->native, 1, &renderTargetHandle, FALSE, NULL);

  /*D3D12_CLEAR_VALUE clearValue;

  D3D12_RENDER_PASS_RENDER_TARGET_DESC rtDesc;
  rtDesc.cpuDescriptor = renderTargetHandle;
  rtDesc.BeginningAccess.Type = D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_CLEAR;
  rtDesc.EndingAccess.Type = D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_PRESERVE;

  ID3D12GraphicsCommandList4_BeginRenderPass(graphicsContext->native, 1, &rtDesc, NULL, D3D12_RENDER_PASS_FLAG_NONE);*/
}

bool CpsrGraphicsContextSetPipelineState(CpsrGraphicsContext *graphicsContext,
                                         CpsrGraphicsPipelineState *pipelineState) {
  assert(graphicsContext);
  assert(pipelineState);
  assert(!graphicsContext->native);

  ID3D12PipelineState *nativePipelineState = CpsrGraphicsPipelineStateGetNative(pipelineState);
  if (!nativePipelineState) {
    // TODO: logo
    return true;
  }

  ID3D12GraphicsCommandList *commandList;
  HRESULT hr = ID3D12Device_CreateCommandList(graphicsContext->commandBuffer->commandQueue->device->nativeDevice,
                                              0,
                                              D3D12_COMMAND_LIST_TYPE_DIRECT,
                                              graphicsContext->commandBuffer->native,
                                              nativePipelineState,
                                              &IID_ID3D12GraphicsCommandList,
                                              &commandList);
  if (FAILED(hr)) {
    return true;
  }

  graphicsContext->native = commandList;
  return false;
}

void CpsrGraphicsContextSetViewport(CpsrGraphicsContext *graphicsContext, CpsrViewport viewport) {
  CPSR_ASSUME(graphicsContext);
  CPSR_ASSUME(graphicsContext->native);

  ID3D12GraphicsCommandList_RSSetViewports(graphicsContext->native, 1, &viewport);
}

void CpsrGraphicsContextSetScissorRect(CpsrGraphicsContext *graphicsContext, CpsrScissorRect scissorRect) {
  CPSR_ASSUME(graphicsContext);
  CPSR_ASSUME(graphicsContext->native);

  ID3D12GraphicsCommandList_RSSetScissorRects(graphicsContext->native, 1, &scissorRect);
}

// ---
// Constant Buffer
// ---
void CpsrGraphicsContextSetConstantBuffer(CpsrGraphicsContext *graphicsContext,
                                          uint8_t index,
                                          const CpsrBuffer *constantBuffer) {
  CPSR_ASSUME(graphicsContext);
  CPSR_ASSUME(graphicsContext->native);
  CPSR_ASSUME(index < CPSR_CONSTANT_BUFFER_COUNT);
  CPSR_ASSUME(constantBuffer);
  CPSR_ASSUME(constantBuffer->bufferType == CPSR_CONSTANT_BUFFER);
  CPSR_ASSUME(graphicsContext->commandBuffer->commandQueue->device == constantBuffer->device);

  D3D12_GPU_VIRTUAL_ADDRESS location = ID3D12Resource_GetGPUVirtualAddress(constantBuffer->native);
  ID3D12GraphicsCommandList_SetGraphicsRootConstantBufferView(graphicsContext->native, index, location);
}

void CpsrGraphicsContextSetConstantBufferWithOffset(CpsrGraphicsContext *graphicsContext,
                                                    uint8_t index,
                                                    const CpsrBuffer *constantBuffer,
                                                    uint32_t offset) {
  CPSR_ASSUME(graphicsContext);
  CPSR_ASSUME(graphicsContext->native);
  CPSR_ASSUME(index < CPSR_CONSTANT_BUFFER_COUNT);
  CPSR_ASSUME(constantBuffer);
  CPSR_ASSUME(constantBuffer->bufferType == CPSR_CONSTANT_BUFFER);
  CPSR_ASSUME(offset % D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT == 0);
  CPSR_ASSUME(graphicsContext->commandBuffer->commandQueue->device == constantBuffer->device);

  D3D12_GPU_VIRTUAL_ADDRESS location = ID3D12Resource_GetGPUVirtualAddress(constantBuffer->native);
  location += offset;
  ID3D12GraphicsCommandList_SetGraphicsRootConstantBufferView(graphicsContext->native, index, location);
}

// ---
// Vertex Buffer
// ---
void CpsrGraphicsContextSetVertexBuffer(CpsrGraphicsContext *graphicsContext,
                                        uint8_t index,
                                        const CpsrBuffer *vertexBuffer) {
  CPSR_ASSUME(graphicsContext);
  CPSR_ASSUME(graphicsContext->native);
  CPSR_ASSUME(index < CPSR_VERTEX_BUFFER_COUNT);
  CPSR_ASSUME(vertexBuffer);
  CPSR_ASSUME(vertexBuffer->bufferType == CPSR_VERTEX_BUFFER);
  CPSR_ASSUME(graphicsContext->commandBuffer->commandQueue->device == vertexBuffer->device);

  D3D12_VERTEX_BUFFER_VIEW vbView;
  vbView.BufferLocation = ID3D12Resource_GetGPUVirtualAddress(vertexBuffer->native);
  vbView.SizeInBytes = (UINT)vertexBuffer->size;
  vbView.StrideInBytes = (UINT)vertexBuffer->size;
  ID3D12GraphicsCommandList_IASetVertexBuffers(graphicsContext->native, index, 1, &vbView);
}

void CpsrGraphicsContextSetVertexBuffers(CpsrGraphicsContext *graphicsContext,
                                         CpsrRange range,
                                         const CpsrBuffer **vertexBuffers) {
  CPSR_ASSUME(graphicsContext);
  CPSR_ASSUME(graphicsContext->native);
  CPSR_ASSUME(range.location + range.length < CPSR_VERTEX_BUFFER_COUNT);
  CPSR_ASSUME(vertexBuffers);
  for (size_t i = 0; i < range.length; ++i) {
    const CpsrBuffer *vertexBuffer = vertexBuffers[i];
    CPSR_ASSUME(vertexBuffer);
    CPSR_ASSUME(vertexBuffer->bufferType == CPSR_VERTEX_BUFFER);
    CPSR_ASSUME(graphicsContext->commandBuffer->commandQueue->device == vertexBuffer->device);
  }

  D3D12_VERTEX_BUFFER_VIEW *vbViews = CpsrAlloc(D3D12_VERTEX_BUFFER_VIEW);
  if (vbViews) {
    for (size_t i = 0; i < range.length; ++i) {
      const CpsrBuffer *vertexBuffer = vertexBuffers[i];
      D3D12_VERTEX_BUFFER_VIEW *vbView = &vbViews[i];
      vbView->BufferLocation = ID3D12Resource_GetGPUVirtualAddress(vertexBuffer->native);
      vbView->SizeInBytes = (UINT)vertexBuffer->size;
      vbView->StrideInBytes = (UINT)vertexBuffer->size;
    }

    ID3D12GraphicsCommandList_IASetVertexBuffers(graphicsContext->native, range.location, range.length, vbViews);

    CpsrDealloc(vbViews);
  } else {
    // TODO log
    exit(0);
  }
}

// ---
// Index Buffer
// ---
void CpsrGraphicsContextSetIndexBuffer(CpsrGraphicsContext *graphicsContext,
                                       const CpsrBuffer *indexBuffer,
                                       CpsrIndexType indexType) {
  CPSR_ASSUME(graphicsContext);
  CPSR_ASSUME(graphicsContext->native);
  CPSR_ASSUME(indexBuffer);
  CPSR_ASSUME(indexBuffer->bufferType == CPSR_VERTEX_BUFFER);
  CPSR_ASSUME(graphicsContext->commandBuffer->commandQueue->device == indexBuffer->device);

  D3D12_INDEX_BUFFER_VIEW ibView;
  ibView.BufferLocation = ID3D12Resource_GetGPUVirtualAddress(indexBuffer->native);
  ibView.SizeInBytes = (UINT)indexBuffer->size;
  ibView.Format = IndexTypeAsDx12Type(indexType);
  ID3D12GraphicsCommandList_IASetIndexBuffer(graphicsContext->native, &ibView);
}

void CpsrGraphicsContextSetIndexBufferWithOffset(CpsrGraphicsContext *graphicsContext,
                                                 const CpsrBuffer *indexBuffer,
                                                 uint32_t offset,
                                                 CpsrIndexType indexType) {
  CPSR_ASSUME(graphicsContext);
  CPSR_ASSUME(graphicsContext->native);
  CPSR_ASSUME(offset % D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT == 0);
  CPSR_ASSUME(indexBuffer);
  CPSR_ASSUME(indexBuffer->bufferType == CPSR_VERTEX_BUFFER);
  CPSR_ASSUME(graphicsContext->commandBuffer->commandQueue->device == indexBuffer->device);

  D3D12_INDEX_BUFFER_VIEW ibView;
  ibView.BufferLocation = ID3D12Resource_GetGPUVirtualAddress(indexBuffer->native) + offset;
  ibView.SizeInBytes = (UINT)indexBuffer->size;
  ibView.Format = IndexTypeAsDx12Type(indexType);
  ID3D12GraphicsCommandList_IASetIndexBuffer(graphicsContext->native, &ibView);
}

// ---
// Texture for pixel shader
// ---
void CpsrGraphicsContextSetPixelTexture(CpsrGraphicsContext *graphicsContext,
                                        uint8_t index,
                                        const CpsrTexture2D *texture) {
  CPSR_ASSUME(graphicsContext);
  CPSR_ASSUME(graphicsContext->native);
  CPSR_ASSUME(texture);
  CPSR_ASSUME(graphicsContext->commandBuffer->commandQueue->device == texture->device);

  D3D12_GPU_VIRTUAL_ADDRESS location = ID3D12Resource_GetGPUVirtualAddress(texture->nativeResource);
  ID3D12GraphicsCommandList_SetGraphicsRootShaderResourceView(graphicsContext->native, index, location);
}

// ---
// Draw
// ---
void CpsrGraphicsContextSetPrimitiveTopology(CpsrGraphicsContext *graphicsContext,
                                             CpsrPrimitiveTopology primitiveTopology) {
  CPSR_ASSUME(graphicsContext);

  D3D12_PRIMITIVE_TOPOLOGY dx12PrimitiveTopology = PrimitiveTopologyAsDx12Type(primitiveTopology);
  ID3D12GraphicsCommandList_IASetPrimitiveTopology(graphicsContext->native, dx12PrimitiveTopology);
}

void CpsrGraphicsContextDraw(const CpsrGraphicsContext *graphicsContext, uint32_t vertexStart, uint32_t vertexCount) {
  CPSR_ASSUME(graphicsContext);
  CPSR_ASSUME(graphicsContext->native);

  ID3D12GraphicsCommandList_DrawInstanced(graphicsContext->native, vertexCount, 1, vertexStart, 0);
}

void CpsrGraphicsContextDrawInstanced(const CpsrGraphicsContext *graphicsContext,
                                      uint32_t vertexStart,
                                      uint32_t vertexCount,
                                      uint32_t instanceCount) {
  CPSR_ASSUME(graphicsContext);
  CPSR_ASSUME(graphicsContext->native);

  ID3D12GraphicsCommandList_DrawInstanced(graphicsContext->native, vertexCount, instanceCount, vertexStart, 0);
}

void CpsrGraphicsContextDrawIndexed(const CpsrGraphicsContext *graphicsContext, uint32_t indexCount) {
  CPSR_ASSUME(graphicsContext);
  CPSR_ASSUME(graphicsContext->native);

  ID3D12GraphicsCommandList_DrawIndexedInstanced(graphicsContext->native, indexCount, 1, 0, 0, 0);
}

void CpsrGraphicsContextDrawIndexedInstanced(const CpsrGraphicsContext *graphicsContext,
                                             uint32_t indexCount,
                                             uint32_t instanceCount,
                                             int32_t baseVertex,
                                             uint32_t baseInstance) {
  CPSR_ASSUME(graphicsContext);
  CPSR_ASSUME(graphicsContext->native);

  ID3D12GraphicsCommandList_DrawIndexedInstanced(
      graphicsContext->native, indexCount, instanceCount, 0, baseVertex, baseInstance);
}

// ---
// End encoding
// ---
void CpsrGraphicsContextClose(const CpsrGraphicsContext *graphicsContext) {
  CPSR_ASSUME(graphicsContext);
  CPSR_ASSUME(graphicsContext->native);

  ID3D12GraphicsCommandList_Close(graphicsContext->native);
}

// ---
// Debug
// ---
#ifndef NDEBUG
// Ref: pix.h
static const UINT PIX_EVENT_UNICODE_VERSION = 0;

void _CpsrGraphicsContextPushDebugGroup(const CpsrGraphicsContext *graphicsContext, const char *groupName) {
  CPSR_ASSUME(graphicsContext);
  CPSR_ASSUME(groupName);

  int utf16GroupNameLength = 0;
  wchar_t *utf16GroupName = CpsrGetUtf16String(groupName, (int)strlen(groupName), &utf16GroupNameLength);
  UINT size = (UINT)(sizeof(wchar_t) * (utf16GroupNameLength + 1));
  ID3D12GraphicsCommandList_BeginEvent(graphicsContext->native, PIX_EVENT_UNICODE_VERSION, utf16GroupName, size);
  free(utf16GroupName);
}

void _CpsrGraphicsContextPopDebugGroup(const CpsrGraphicsContext *graphicsContext) {
  CPSR_ASSUME(graphicsContext);

  ID3D12GraphicsCommandList_EndEvent(graphicsContext->native);
}
#endif
