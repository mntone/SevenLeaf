#include "CpsrGraphics+Private.h"

struct _CpsrGraphicsPipelineState {
  const CpsrDevice *device;
  D3D12_GRAPHICS_PIPELINE_STATE_DESC desc;
  ID3D12PipelineState *native;
};


// clang-format off
static inline CpsrBlendFactor BlendFactorAsCpsrType(D3D12_BLEND blendFactor) {
  switch (blendFactor) {
  case D3D12_BLEND_ZERO:             return CPSR_BLEND_ZERO;
  case D3D12_BLEND_ONE:              return CPSR_BLEND_ONE;
  case D3D12_BLEND_SRC_COLOR:        return CPSR_BLEND_SOURCE_COLOR;
  case D3D12_BLEND_INV_SRC_COLOR:    return CPSR_BLEND_ONE_MINUS_SOURCE_COLOR;
  case D3D12_BLEND_SRC_ALPHA:        return CPSR_BLEND_SOURCE_ALPHA;
  case D3D12_BLEND_INV_SRC_ALPHA:    return CPSR_BLEND_ONE_MINUS_SOURCE_ALPHA;
  case D3D12_BLEND_DEST_ALPHA:       return CPSR_BLEND_DESTINATION_COLOR;
  case D3D12_BLEND_INV_DEST_ALPHA:   return CPSR_BLEND_ONE_MINUS_DESTINATION_COLOR;
  case D3D12_BLEND_DEST_COLOR:       return CPSR_BLEND_DESTINATION_ALPHA;
  case D3D12_BLEND_INV_DEST_COLOR:   return CPSR_BLEND_ONE_MINUS_DESTINATION_ALPHA;
  case D3D12_BLEND_SRC_ALPHA_SAT:    return CPSR_BLEND_SOURCE_ALPHA_SATURATED;
  case D3D12_BLEND_BLEND_FACTOR:     return CPSR_BLEND_BLEND_FACTOR;
  case D3D12_BLEND_INV_BLEND_FACTOR: return CPSR_BLEND_ONE_MINUS_BLEND_FACTOR;
  case D3D12_BLEND_SRC1_COLOR:       return CPSR_BLEND_SOURCE1_COLOR;
  case D3D12_BLEND_INV_SRC1_COLOR:   return CPSR_BLEND_ONE_MINUS_SOURCE1_COLOR;
  case D3D12_BLEND_SRC1_ALPHA:       return CPSR_BLEND_SOURCE1_ALPHA;
  case D3D12_BLEND_INV_SRC1_ALPHA:   return CPSR_BLEND_ONE_MINUS_SOURCE1_ALPHA;
  default:                           return CPSR_BLEND_UNKNOWN;
  }
}

static inline D3D12_BLEND BlendFactorAsDx12Type(CpsrBlendFactor blendFactor, D3D12_BLEND defaultBlendFactor) {
  switch (blendFactor) {
  case CPSR_BLEND_ZERO:                        return D3D12_BLEND_ZERO;
  case CPSR_BLEND_ONE:                         return D3D12_BLEND_ONE;
  case CPSR_BLEND_SOURCE_COLOR:                return D3D12_BLEND_SRC_COLOR;
  case CPSR_BLEND_ONE_MINUS_SOURCE_COLOR:      return D3D12_BLEND_INV_SRC_COLOR;
  case CPSR_BLEND_SOURCE_ALPHA:                return D3D12_BLEND_SRC_ALPHA;
  case CPSR_BLEND_ONE_MINUS_SOURCE_ALPHA:      return D3D12_BLEND_INV_SRC_ALPHA;
  case CPSR_BLEND_DESTINATION_COLOR:           return D3D12_BLEND_DEST_ALPHA;
  case CPSR_BLEND_ONE_MINUS_DESTINATION_COLOR: return D3D12_BLEND_INV_DEST_ALPHA;
  case CPSR_BLEND_DESTINATION_ALPHA:           return D3D12_BLEND_DEST_COLOR;
  case CPSR_BLEND_ONE_MINUS_DESTINATION_ALPHA: return D3D12_BLEND_INV_DEST_COLOR;
  case CPSR_BLEND_SOURCE_ALPHA_SATURATED:      return D3D12_BLEND_SRC_ALPHA_SAT;
  case CPSR_BLEND_BLEND_FACTOR:                return D3D12_BLEND_BLEND_FACTOR;
  case CPSR_BLEND_ONE_MINUS_BLEND_FACTOR:      return D3D12_BLEND_INV_BLEND_FACTOR;
  case CPSR_BLEND_SOURCE1_COLOR:               return D3D12_BLEND_SRC1_COLOR;
  case CPSR_BLEND_ONE_MINUS_SOURCE1_COLOR:     return D3D12_BLEND_INV_SRC1_COLOR;
  case CPSR_BLEND_SOURCE1_ALPHA:               return D3D12_BLEND_SRC1_ALPHA;
  case CPSR_BLEND_ONE_MINUS_SOURCE1_ALPHA:     return D3D12_BLEND_INV_SRC1_ALPHA;
  default:                                     return defaultBlendFactor;
  }
}

static inline CpsrBlendOperation BlendOperationAsCpsrType(D3D12_BLEND_OP blendOperation) {
  switch (blendOperation) {
  case D3D12_BLEND_OP_SUBTRACT:     return CPSR_BLEND_OPERATION_SUBTRACT;
  case D3D12_BLEND_OP_REV_SUBTRACT: return CPSR_BLEND_OPERATION_REVERSE_SUBTRACT;
  case D3D12_BLEND_OP_MIN:          return CPSR_BLEND_OPERATION_MIN;
  case D3D12_BLEND_OP_MAX:          return CPSR_BLEND_OPERATION_MAX;
  case D3D12_BLEND_OP_ADD:
  default:                          return CPSR_BLEND_OPERATION_ADD;
  }
}

static inline D3D12_BLEND_OP BlendOperationAsDx12Type(CpsrBlendOperation blendOperation) {
  switch (blendOperation) {
  case CPSR_BLEND_OPERATION_SUBTRACT:         return D3D12_BLEND_OP_SUBTRACT;
  case CPSR_BLEND_OPERATION_REVERSE_SUBTRACT: return D3D12_BLEND_OP_REV_SUBTRACT;
  case CPSR_BLEND_OPERATION_MIN:              return D3D12_BLEND_OP_MIN;
  case CPSR_BLEND_OPERATION_MAX:              return D3D12_BLEND_OP_MAX;
  case CPSR_BLEND_OPERATION_ADD:
  default:                                    return D3D12_BLEND_OP_ADD;
  }
}
static inline CpsrBlendFactor FillModeAsCpsrType(D3D12_FILL_MODE fillMode) {
  switch (fillMode) {
  case D3D12_FILL_MODE_WIREFRAME: return CPSR_FILL_WIREFRAME;
  case D3D12_FILL_MODE_SOLID:
  default:                        return CPSR_FILL_SOLID;
  }
}

static inline D3D12_FILL_MODE FillModeAsDx12Type(CpsrFillMode fillMode) {
  switch (fillMode) {
  case CPSR_FILL_WIREFRAME: return D3D12_FILL_MODE_WIREFRAME;
  case CPSR_FILL_SOLID:
  default:                  return D3D12_FILL_MODE_SOLID;
  }
}

static inline CpsrCullMode CullModeAsCpsrType(D3D12_CULL_MODE cullMode) {
  switch (cullMode) {
  case D3D12_CULL_MODE_NONE:  return CPSR_CULL_NONE;
  case D3D12_CULL_MODE_FRONT: return CPSR_CULL_FRONT;
  case D3D12_CULL_MODE_BACK:
  default:                    return CPSR_CULL_BACK;
  }
}

static inline D3D12_CULL_MODE CullModeAsDx12Type(CpsrCullMode cullMode) {
  switch (cullMode) {
  case CPSR_CULL_NONE:  return D3D12_CULL_MODE_NONE;
  case CPSR_CULL_FRONT: return D3D12_CULL_MODE_FRONT;
  case CPSR_CULL_BACK:
  default:              return D3D12_CULL_MODE_BACK;
  }
}

static inline CpsrDepthClipMode DepthClipModeAsCpsrType(BOOL depthClipEnable) {
  return depthClipEnable ? CPSR_DEPTH_CLIP_CLIP : CPSR_DEPTH_CLIP_CLAMP;
}

static inline BOOL DepthClipModeAsDx12Type(CpsrDepthClipMode depthClipMode) {
  switch (depthClipMode) {
  case CPSR_DEPTH_CLIP_CLAMP: return FALSE;
  case CPSR_DEPTH_CLIP_CLIP:
  default:                    return TRUE;
  }
}
// clang-format on

CpsrGraphicsPipelineState *CpsrGraphicsPipelineStateCreate(const CpsrDevice *device) {
  CPSR_ASSUME(device);

  CpsrGraphicsPipelineState *pipelineState = CpsrAlloc(CpsrGraphicsPipelineState);
  if (!pipelineState) {
    return NULL;
  }

  pipelineState->device = device;
  memset(&pipelineState->desc, 0, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
  pipelineState->desc.SampleMask = UINT_MAX;
  pipelineState->native = NULL;
  return pipelineState;
}

static inline void CpsrPipelineStateRelease(CpsrGraphicsPipelineState *pipelineState) {
  ID3D12PipelineState *native = pipelineState->native;
  if (native) {
    ID3D12PipelineState_Release(native);
    pipelineState->native = NULL;
  }
}

void CpsrGraphicsPipelineStateDestroy(CpsrGraphicsPipelineState *pipelineState) {
  CPSR_ASSUME(pipelineState);

  CpsrPipelineStateRelease(pipelineState);
  CpsrDealloc(pipelineState);
}

ID3D12PipelineState *CpsrGraphicsPipelineStateGetNative(CpsrGraphicsPipelineState *pipelineState) {
  ID3D12PipelineState *native = pipelineState->native;
  if (!native) {
    HRESULT hr = ID3D12Device_CreateGraphicsPipelineState(
        pipelineState->device->nativeDevice, &pipelineState->desc, &IID_ID3D12PipelineState, &native);
    if (FAILED(hr)) {
      // TODO: log
    }

    pipelineState->native = native;
  }
  return native;
}

static inline void UpdateRenderTargetCount(CpsrGraphicsPipelineState *pipelineState, uint8_t index) {
  CPSR_ASSUME(pipelineState);

  uint32_t newCount = index + 1;
  if (pipelineState->desc.NumRenderTargets < newCount) {
    pipelineState->desc.NumRenderTargets = newCount;
  }
}

// ---
// Property: Vertex Function
// ---
void CpsrGraphicsPipelineStateSetVertexFunction(CpsrGraphicsPipelineState *pipelineState,
                                                const CpsrShaderFunction *shaderFunction) {
  CPSR_ASSUME(pipelineState);
  CPSR_ASSUME(shaderFunction);
  CpsrPipelineStateRelease(pipelineState);

  if (shaderFunction) {
    pipelineState->desc.VS.pShaderBytecode = ID3D10Blob_GetBufferPointer(shaderFunction->native);
    pipelineState->desc.VS.BytecodeLength = ID3D10Blob_GetBufferSize(shaderFunction->native);
  }
}

// ---
// Property: Pixel Function
// ---
void CpsrGraphicsPipelineStateSetPixelFunction(CpsrGraphicsPipelineState *pipelineState,
                                               const CpsrShaderFunction *shaderFunction) {
  CPSR_ASSUME(pipelineState);
  CPSR_ASSUME(shaderFunction);
  CpsrPipelineStateRelease(pipelineState);

  if (shaderFunction) {
    pipelineState->desc.PS.pShaderBytecode = ID3D10Blob_GetBufferPointer(shaderFunction->native);
    pipelineState->desc.PS.BytecodeLength = ID3D10Blob_GetBufferSize(shaderFunction->native);
  }
}

// ---
// Property: Blend state
// ---
void CpsrGraphicsPipelineStateGetBlendState(const CpsrGraphicsPipelineState *pipelineState,
                                            uint8_t index,
                                            CpsrBlendDescriptor *blendState) {
  CPSR_ASSUME(pipelineState);
  CPSR_ASSUME(blendState);
  CPSR_ASSUME(index < 8);

  const D3D12_RENDER_TARGET_BLEND_DESC *blendDesc = &pipelineState->desc.BlendState.RenderTarget[index];
  blendState->blendEnable = blendDesc->BlendEnable;
  blendState->srcColorBlend = BlendFactorAsCpsrType(blendDesc->SrcBlend);
  blendState->dstColorBlend = BlendFactorAsCpsrType(blendDesc->DestBlend);
  blendState->colorOperation = BlendOperationAsCpsrType(blendDesc->BlendOp);
  blendState->srcAlphaBlend = BlendFactorAsCpsrType(blendDesc->SrcBlendAlpha);
  blendState->dstAlphaBlend = BlendFactorAsCpsrType(blendDesc->DestBlendAlpha);
  blendState->alphaOperation = BlendOperationAsCpsrType(blendDesc->BlendOpAlpha);
  blendState->writeMask = blendDesc->RenderTargetWriteMask;
}

void CpsrGraphicsPipelineStateSetBlendState(CpsrGraphicsPipelineState *pipelineState,
                                            uint8_t index,
                                            const CpsrBlendDescriptor *blendState) {
  CPSR_ASSUME(pipelineState);
  CPSR_ASSUME(blendState);
  CPSR_ASSUME(index < 8);
  CpsrPipelineStateRelease(pipelineState);
  UpdateRenderTargetCount(pipelineState, index);

  D3D12_RENDER_TARGET_BLEND_DESC *blendDesc = &pipelineState->desc.BlendState.RenderTarget[index];
  blendDesc->BlendEnable = blendState->blendEnable;
  blendDesc->SrcBlend = BlendFactorAsDx12Type(blendState->srcColorBlend, D3D12_BLEND_ONE);
  blendDesc->DestBlend = BlendFactorAsDx12Type(blendState->dstColorBlend, D3D12_BLEND_ZERO);
  blendDesc->BlendOp = BlendOperationAsDx12Type(blendState->colorOperation);
  blendDesc->SrcBlendAlpha = BlendFactorAsDx12Type(blendState->srcAlphaBlend, D3D12_BLEND_ONE);
  blendDesc->DestBlendAlpha = BlendFactorAsDx12Type(blendState->dstAlphaBlend, D3D12_BLEND_ZERO);
  blendDesc->BlendOpAlpha = BlendOperationAsDx12Type(blendState->alphaOperation);
  blendDesc->RenderTargetWriteMask = blendState->writeMask;
}

// ---
// Property: Rasterizer state
// ---
void CpsrGraphicsPipelineStateGetRasterizerState(const CpsrGraphicsPipelineState *pipelineState,
                                                 CpsrRasterizerStateDescriptor *rasterizerState) {
  CPSR_ASSUME(pipelineState);
  CPSR_ASSUME(rasterizerState);

  const D3D12_RASTERIZER_DESC *rasterizerDesc = &pipelineState->desc.RasterizerState;
  rasterizerState->fillMode = FillModeAsCpsrType(rasterizerDesc->FillMode);
  rasterizerState->cullMode = CullModeAsCpsrType(rasterizerDesc->CullMode);
  rasterizerState->counterClockwise = rasterizerDesc->FrontCounterClockwise;
  rasterizerState->depthClipMode = DepthClipModeAsCpsrType(rasterizerDesc->DepthClipEnable);
  rasterizerState->depthBias = rasterizerDesc->DepthBias;
  rasterizerState->depthBiasClamp = rasterizerDesc->DepthBiasClamp;
  rasterizerState->slopeScaledDepthBias = rasterizerDesc->SlopeScaledDepthBias;
  rasterizerState->rasterSampleCount = rasterizerDesc->ForcedSampleCount;
}

void CpsrGraphicsPipelineStateSetRasterizerState(CpsrGraphicsPipelineState *pipelineState,
                                                 const CpsrRasterizerStateDescriptor *rasterizerState) {
  CPSR_ASSUME(pipelineState);
  CPSR_ASSUME(rasterizerState);
  CpsrPipelineStateRelease(pipelineState);

  D3D12_RASTERIZER_DESC *rasterizerDesc = &pipelineState->desc.RasterizerState;
  rasterizerDesc->FillMode = FillModeAsDx12Type(rasterizerState->fillMode);
  rasterizerDesc->CullMode = CullModeAsDx12Type(rasterizerState->cullMode);
  rasterizerDesc->FrontCounterClockwise = rasterizerState->counterClockwise;
  rasterizerDesc->DepthBias = rasterizerState->depthBias;
  rasterizerDesc->DepthBiasClamp = rasterizerState->depthBiasClamp;
  rasterizerDesc->SlopeScaledDepthBias = rasterizerState->slopeScaledDepthBias;
  rasterizerDesc->DepthClipEnable = DepthClipModeAsDx12Type(rasterizerState->depthClipMode);
  rasterizerDesc->ForcedSampleCount = rasterizerState->rasterSampleCount;
}

// ---
// Property: Primitive Topology Type
// ---
// clang-format off
static inline CpsrPrimitiveTopologyType PrimitiveTopologyTypeAsCpsrType(D3D12_PRIMITIVE_TOPOLOGY_TYPE primitiveTopologyType) {
  switch (primitiveTopologyType) {
  case D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT:     return CPSR_PRIMITIVE_TOPOLOGY_TYPE_POINT;
  case D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE:      return CPSR_PRIMITIVE_TOPOLOGY_TYPE_LINE;
  case D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE:  return CPSR_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
  case D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED:
  default:                                      return CPSR_PRIMITIVE_TOPOLOGY_TYPE_UNSPECIFIED;
  }
}

static inline D3D12_PRIMITIVE_TOPOLOGY_TYPE PrimitiveTopologyTypeAsDx12Type(CpsrPrimitiveTopologyType primitiveTopologyType) {
  switch (primitiveTopologyType) {
  case CPSR_PRIMITIVE_TOPOLOGY_TYPE_POINT:       return D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
  case CPSR_PRIMITIVE_TOPOLOGY_TYPE_LINE:        return D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
  case CPSR_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE:    return D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
  case CPSR_PRIMITIVE_TOPOLOGY_TYPE_UNSPECIFIED:
  default:                                       return D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED;
  }
}
// clang-format on

CpsrPrimitiveTopologyType CpsrGraphicsPipelineStateGetPrimitiveTopologyType(
    const CpsrGraphicsPipelineState *pipelineState) {
  CPSR_ASSUME(pipelineState);

  return PrimitiveTopologyTypeAsCpsrType(pipelineState->desc.PrimitiveTopologyType);
}

void CpsrGraphicsPipelineStateSetPrimitiveTopologyType(CpsrGraphicsPipelineState *pipelineState,
                                                       CpsrPrimitiveTopologyType primitiveTopologyType) {
  CPSR_ASSUME(pipelineState);
  CpsrPipelineStateRelease(pipelineState);

  pipelineState->desc.PrimitiveTopologyType = PrimitiveTopologyTypeAsDx12Type(primitiveTopologyType);
}

// ---
// Property: Multisample count
// ---
uint8_t CpsrGraphicsPipelineStateGetMultisampleCount(const CpsrGraphicsPipelineState *pipelineState) {
  CPSR_ASSUME(pipelineState);

  return (uint8_t)pipelineState->desc.SampleDesc.Count;
}

void CpsrGraphicsPipelineStateSetMultisampleCount(CpsrGraphicsPipelineState *pipelineState, uint8_t multisampleCount) {
  CPSR_ASSUME(pipelineState);
  CPSR_ASSUME(multisampleCount > 0);
  CPSR_ASSUME(multisampleCount <= 16);
  CpsrPipelineStateRelease(pipelineState);

  if (multisampleCount > 1) {
    pipelineState->desc.RasterizerState.MultisampleEnable = TRUE;
    pipelineState->desc.SampleDesc.Count = multisampleCount;
  } else {
    pipelineState->desc.RasterizerState.MultisampleEnable = FALSE;
    pipelineState->desc.SampleDesc.Count = 1;
  }
}

// ---
// Property: Render Target Pixel Format
// ---
void CpsrSetRenderTargetPixelFormat(CpsrGraphicsPipelineState *pipelineState,
                                    uint8_t index,
                                    CpsrPixelFormat pixelFormat) {
  CPSR_ASSUME(pipelineState);
  CPSR_ASSUME(index < 8);
  CpsrPipelineStateRelease(pipelineState);
  UpdateRenderTargetCount(pipelineState, index);

  pipelineState->desc.RTVFormats[index] = PixelFormatAsDx12Type(pixelFormat);
}

void CpsrSetRenderTargetPixelFormatFromTexture2D(CpsrGraphicsPipelineState *pipelineState,
                                                 uint8_t index,
                                                 const CpsrTexture2D *renderTarget) {
  CPSR_ASSUME(pipelineState);
  CPSR_ASSUME(index < 8);
  CPSR_ASSUME(renderTarget);
  UpdateRenderTargetCount(pipelineState, index);

  pipelineState->desc.RTVFormats[index] = renderTarget->srvDesc.Format;
}

void CpsrSetRenderTargetPixelFormatFromSwapChain(CpsrGraphicsPipelineState *pipelineState,
                                                 uint8_t index,
                                                 CpsrSwapChain *swapChain) {
  CPSR_ASSUME(pipelineState);
  CPSR_ASSUME(index < 8);
  CPSR_ASSUME(swapChain);

  pipelineState->desc.RTVFormats[index] = swapChain->pixelFormat;
}
