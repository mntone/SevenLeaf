#include "CpsrGraphics+Private.h"

// clang-format off
static inline CpsrBlendFactor BlendFactorAsCpsrType(MTLBlendFactor blendFactor) {
  switch (blendFactor) {
  case MTLBlendFactorZero:                     return CPSR_BLEND_ZERO;
  case MTLBlendFactorOne:                      return CPSR_BLEND_ONE;
  case MTLBlendFactorSourceColor:              return CPSR_BLEND_SOURCE_COLOR;
  case MTLBlendFactorOneMinusSourceColor:      return CPSR_BLEND_ONE_MINUS_SOURCE_COLOR;
  case MTLBlendFactorSourceAlpha:              return CPSR_BLEND_SOURCE_ALPHA;
  case MTLBlendFactorOneMinusSourceAlpha:      return CPSR_BLEND_ONE_MINUS_SOURCE_ALPHA;
  case MTLBlendFactorDestinationColor:         return CPSR_BLEND_DESTINATION_COLOR;
  case MTLBlendFactorOneMinusDestinationColor: return CPSR_BLEND_ONE_MINUS_DESTINATION_COLOR;
  case MTLBlendFactorDestinationAlpha:         return CPSR_BLEND_DESTINATION_ALPHA;
  case MTLBlendFactorOneMinusDestinationAlpha: return CPSR_BLEND_ONE_MINUS_DESTINATION_ALPHA;
  case MTLBlendFactorSourceAlphaSaturated:     return CPSR_BLEND_SOURCE_ALPHA_SATURATED;
  case MTLBlendFactorBlendColor:               return CPSR_BLEND_BLEND_FACTOR;
  case MTLBlendFactorOneMinusBlendColor:       return CPSR_BLEND_ONE_MINUS_BLEND_FACTOR;
  case MTLBlendFactorBlendAlpha:               return CPSR_BLEND_BLEND_FACTOR;
  case MTLBlendFactorOneMinusBlendAlpha:       return CPSR_BLEND_ONE_MINUS_BLEND_FACTOR;
  case MTLBlendFactorSource1Color:             return CPSR_BLEND_SOURCE1_COLOR;
  case MTLBlendFactorOneMinusSource1Color:     return CPSR_BLEND_ONE_MINUS_SOURCE1_COLOR;
  case MTLBlendFactorSource1Alpha:             return CPSR_BLEND_SOURCE1_ALPHA;
  case MTLBlendFactorOneMinusSource1Alpha:     return CPSR_BLEND_ONE_MINUS_SOURCE1_ALPHA;
  default:                                     return CPSR_BLEND_UNKNOWN;
  }
}

static inline MTLBlendFactor BlendFactorAsMetalType(CpsrBlendFactor blendFactor, MTLBlendFactor defaultBlendFactor, bool isColor) {
  switch (blendFactor) {
  case CPSR_BLEND_ZERO:                        return MTLBlendFactorZero;
  case CPSR_BLEND_ONE:                         return MTLBlendFactorOne;
  case CPSR_BLEND_SOURCE_COLOR:                return MTLBlendFactorSourceColor;
  case CPSR_BLEND_ONE_MINUS_SOURCE_COLOR:      return MTLBlendFactorOneMinusSourceColor;
  case CPSR_BLEND_SOURCE_ALPHA:                return MTLBlendFactorSourceAlpha;
  case CPSR_BLEND_ONE_MINUS_SOURCE_ALPHA:      return MTLBlendFactorOneMinusSourceAlpha;
  case CPSR_BLEND_DESTINATION_COLOR:           return MTLBlendFactorDestinationColor;
  case CPSR_BLEND_ONE_MINUS_DESTINATION_COLOR: return MTLBlendFactorOneMinusDestinationColor;
  case CPSR_BLEND_DESTINATION_ALPHA:           return MTLBlendFactorDestinationAlpha;
  case CPSR_BLEND_ONE_MINUS_DESTINATION_ALPHA: return MTLBlendFactorOneMinusDestinationAlpha;
  case CPSR_BLEND_SOURCE_ALPHA_SATURATED:      return MTLBlendFactorSourceAlphaSaturated;
  case CPSR_BLEND_BLEND_FACTOR:                return isColor ? MTLBlendFactorBlendColor : MTLBlendFactorBlendAlpha;
  case CPSR_BLEND_ONE_MINUS_BLEND_FACTOR:      return isColor ? MTLBlendFactorOneMinusBlendColor : MTLBlendFactorOneMinusBlendAlpha;
  case CPSR_BLEND_SOURCE1_COLOR:               return MTLBlendFactorSource1Color;
  case CPSR_BLEND_ONE_MINUS_SOURCE1_COLOR:     return MTLBlendFactorOneMinusSource1Color;
  case CPSR_BLEND_SOURCE1_ALPHA:               return MTLBlendFactorSource1Alpha;
  case CPSR_BLEND_ONE_MINUS_SOURCE1_ALPHA:     return MTLBlendFactorOneMinusSource1Alpha;
  default:                                     return defaultBlendFactor;
  }
}

static inline CpsrBlendOperation BlendOperationAsCpsrType(MTLBlendOperation blendOperation) {
  switch (blendOperation) {
  case MTLBlendOperationSubtract:        return CPSR_BLEND_OPERATION_SUBTRACT;
  case MTLBlendOperationReverseSubtract: return CPSR_BLEND_OPERATION_REVERSE_SUBTRACT;
  case MTLBlendOperationMin:             return CPSR_BLEND_OPERATION_MIN;
  case MTLBlendOperationMax:             return CPSR_BLEND_OPERATION_MAX;
  case MTLBlendOperationAdd:
  default:                               return CPSR_BLEND_OPERATION_ADD;
  }
}

static inline MTLBlendOperation BlendOperationAsMetalType(CpsrBlendOperation blendOperation) {
  switch (blendOperation) {
  case CPSR_BLEND_OPERATION_SUBTRACT:         return MTLBlendOperationSubtract;
  case CPSR_BLEND_OPERATION_REVERSE_SUBTRACT: return MTLBlendOperationReverseSubtract;
  case CPSR_BLEND_OPERATION_MIN:              return MTLBlendOperationMin;
  case CPSR_BLEND_OPERATION_MAX:              return MTLBlendOperationMax;
  case CPSR_BLEND_OPERATION_ADD:
  default:                                    return MTLBlendOperationAdd;
  }
}

static inline uint8_t WriteMaskAsCpsrType(uint8_t writeMask) {
  uint8_t ret = 0;
  if (writeMask & MTLColorWriteMaskRed) ret |= 0x1;
  if (writeMask & MTLColorWriteMaskGreen) ret |= 0x2;
  if (writeMask & MTLColorWriteMaskBlue) ret |= 0x4;
  if (writeMask & MTLColorWriteMaskAlpha) ret |= 0x8;
  return ret;
}

static inline MTLColorWriteMask WriteMaskAsMetalType(uint8_t writeMask) {
  MTLColorWriteMask ret = MTLColorWriteMaskNone;
  if (writeMask & 0x1) ret |= MTLColorWriteMaskRed;
  if (writeMask & 0x2) ret |= MTLColorWriteMaskGreen;
  if (writeMask & 0x4) ret |= MTLColorWriteMaskBlue;
  if (writeMask & 0x8) ret |= MTLColorWriteMaskAlpha;
  return ret;
}
// clang-format on

CpsrGraphicsPipelineState *CpsrGraphicsPipelineStateCreate(const CpsrDevice *device) {
  CPSR_ASSUME(device);
  
  CpsrGraphicsPipelineState *pipelineState = CpsrAlloc(CpsrGraphicsPipelineState);
  if (pipelineState) {
    pipelineState->device         = device;
    pipelineState->desc           = [[MTLRenderPipelineDescriptor alloc] init];
    pipelineState->rasterizerDesc = kCpsrRasterizerDefault;
    pipelineState->native         = nil;
  }
  return pipelineState;
}

static inline void CpsrPipelineStateRelease(CpsrGraphicsPipelineState *pipelineState) {
  id<MTLRenderPipelineState> native = pipelineState->native;
  if (native) {
    [native release];
    pipelineState->native = nil;
  }
}

void CpsrGraphicsPipelineStateDestroy(CpsrGraphicsPipelineState *pipelineState) {
  CPSR_ASSUME(pipelineState);
  
  CpsrPipelineStateRelease(pipelineState);
  [pipelineState->desc release];
  CpsrDealloc(pipelineState);
}

id<MTLRenderPipelineState> CpsrGraphicsPipelineStateGetNative(CpsrGraphicsPipelineState *pipelineState) {
  id<MTLRenderPipelineState> native = pipelineState->native;
  if (!native) {
    NSError *error = nil;
    native = [pipelineState->device->native newRenderPipelineStateWithDescriptor:pipelineState->desc error:&error];
    
    if (error) {
      // TODO: log
      [error release];
    } else {
      pipelineState->native = native;
    }
  }
  return native;
}

// ---
// Property: Vertex Function
// ---
void CpsrGraphicsPipelineStateSetVertexFunction(CpsrGraphicsPipelineState *pipelineState, const CpsrShaderFunction *shaderFunction) {
  CPSR_ASSUME(pipelineState);
  CPSR_ASSUME(shaderFunction);
  CpsrPipelineStateRelease(pipelineState);
  
  if (shaderFunction) {
    assert(shaderFunction->native.functionType == MTLFunctionTypeVertex);
    pipelineState->desc.vertexFunction = shaderFunction->native;
  }
}

// ---
// Property: Pixel Function
// ---
void CpsrGraphicsPipelineStateSetPixelFunction(CpsrGraphicsPipelineState *pipelineState, const CpsrShaderFunction *shaderFunction) {
  CPSR_ASSUME(pipelineState);
  CPSR_ASSUME(shaderFunction);
  CpsrPipelineStateRelease(pipelineState);
    
  if (shaderFunction) {
    assert(shaderFunction->native.functionType == MTLFunctionTypeFragment);
    pipelineState->desc.fragmentFunction = shaderFunction->native;
  }
}

// ---
// Property: Blend state
// ---
void CpsrGraphicsPipelineStateGetBlendState(const CpsrGraphicsPipelineState *pipelineState, uint8_t index, CpsrBlendDescriptor *blendState) {
  CPSR_ASSUME(pipelineState);
  CPSR_ASSUME(blendState);
  CPSR_ASSUME(index < 8);
  
  MTLRenderPipelineColorAttachmentDescriptor *colorAttachment = pipelineState->desc.colorAttachments[index];
  blendState->blendEnable = colorAttachment.blendingEnabled;
  blendState->srcColorBlend = BlendFactorAsCpsrType(colorAttachment.sourceRGBBlendFactor);
  blendState->dstColorBlend = BlendFactorAsCpsrType(colorAttachment.destinationRGBBlendFactor);
  blendState->colorOperation = BlendOperationAsCpsrType(colorAttachment.rgbBlendOperation);
  blendState->srcAlphaBlend = BlendFactorAsCpsrType(colorAttachment.sourceAlphaBlendFactor);
  blendState->dstAlphaBlend = BlendFactorAsCpsrType(colorAttachment.destinationAlphaBlendFactor);
  blendState->alphaOperation = BlendOperationAsCpsrType(colorAttachment.alphaBlendOperation);
  blendState->writeMask = WriteMaskAsCpsrType(colorAttachment.writeMask);
}

void CpsrGraphicsPipelineStateSetBlendState(CpsrGraphicsPipelineState *pipelineState, uint8_t index, const CpsrBlendDescriptor *blendState) {
  CPSR_ASSUME(pipelineState);
  CPSR_ASSUME(blendState);
  CPSR_ASSUME(index < 8);
  CpsrPipelineStateRelease(pipelineState);
  
  MTLRenderPipelineColorAttachmentDescriptor *colorAttachment = pipelineState->desc.colorAttachments[index];
  colorAttachment.blendingEnabled = blendState->blendEnable;
  colorAttachment.sourceRGBBlendFactor = BlendFactorAsMetalType(blendState->srcColorBlend, MTLBlendFactorOne, true);
  colorAttachment.destinationRGBBlendFactor = BlendFactorAsMetalType(blendState->dstColorBlend, MTLBlendFactorZero, true);
  colorAttachment.rgbBlendOperation = BlendOperationAsMetalType(blendState->colorOperation);
  colorAttachment.sourceAlphaBlendFactor = BlendFactorAsMetalType(blendState->srcAlphaBlend, MTLBlendFactorOne, false);
  colorAttachment.destinationAlphaBlendFactor = BlendFactorAsMetalType(blendState->dstAlphaBlend, MTLBlendFactorZero, false);
  colorAttachment.alphaBlendOperation = BlendOperationAsMetalType(blendState->alphaOperation);
  colorAttachment.writeMask = WriteMaskAsMetalType(blendState->writeMask);
}

// ---
// Property: Rasterizer state
// ---
void CpsrGraphicsPipelineStateGetRasterizerState(const CpsrGraphicsPipelineState *pipelineState, CpsrRasterizerDescriptor *rasterizerState) {
  CPSR_ASSUME(rasterizerState);
  
  memcpy(rasterizerState, &pipelineState->rasterizerDesc, sizeof(CpsrRasterizerDescriptor));
  rasterizerState->rasterSampleCount = pipelineState->desc.rasterSampleCount;
}

void CpsrGraphicsPipelineStateSetRasterizerState(CpsrGraphicsPipelineState *pipelineState, const CpsrRasterizerDescriptor *rasterizerState) {
  CPSR_ASSUME(pipelineState);
  CPSR_ASSUME(rasterizerState);
  CpsrPipelineStateRelease(pipelineState);
  
  memcpy(&pipelineState->rasterizerDesc, rasterizerState, sizeof(CpsrRasterizerDescriptor));
  pipelineState->desc.rasterSampleCount = rasterizerState->rasterSampleCount;
}

// ---
// Property: Primitive Topology Type
// ---
// clang-format off
static inline CpsrPrimitiveTopologyType PrimitiveTopologyTypeAsCpsrType(MTLPrimitiveTopologyClass primitiveTopologyType) {
  switch (primitiveTopologyType) {
  case MTLPrimitiveTopologyClassPoint:       return CPSR_PRIMITIVE_TOPOLOGY_TYPE_POINT;
  case MTLPrimitiveTopologyClassLine:        return CPSR_PRIMITIVE_TOPOLOGY_TYPE_LINE;
  case MTLPrimitiveTopologyClassTriangle:    return CPSR_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
  case MTLPrimitiveTopologyClassUnspecified:
  default:                                   return CPSR_PRIMITIVE_TOPOLOGY_TYPE_UNSPECIFIED;
  }
}

static inline MTLPrimitiveTopologyClass PrimitiveTopologyTypeAsMetalType(CpsrPrimitiveTopologyType primitiveTopologyType) {
  switch (primitiveTopologyType) {
  case CPSR_PRIMITIVE_TOPOLOGY_TYPE_POINT:       return MTLPrimitiveTopologyClassPoint;
  case CPSR_PRIMITIVE_TOPOLOGY_TYPE_LINE:        return MTLPrimitiveTopologyClassLine;
  case CPSR_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE:    return MTLPrimitiveTopologyClassTriangle;
  case CPSR_PRIMITIVE_TOPOLOGY_TYPE_UNSPECIFIED:
  default:                                       return MTLPrimitiveTopologyClassUnspecified;
  }
}
// clang-format on

CpsrPrimitiveTopologyType CpsrGraphicsPipelineStateGetPrimitiveTopologyType(const CpsrGraphicsPipelineState *pipelineState) {
  CPSR_ASSUME(pipelineState);

  return PrimitiveTopologyTypeAsCpsrType(pipelineState->desc.inputPrimitiveTopology);
}

void CpsrGraphicsPipelineStateSetPrimitiveTopologyType(CpsrGraphicsPipelineState *pipelineState,
                                                       CpsrPrimitiveTopologyType primitiveTopologyType) {
  CPSR_ASSUME(pipelineState);
  CpsrPipelineStateRelease(pipelineState);

  pipelineState->desc.inputPrimitiveTopology = PrimitiveTopologyTypeAsMetalType(primitiveTopologyType);
}

// ---
// Property: Multisample count
// ---
uint8_t CpsrGraphicsPipelineStateGetMultisampleCount(const CpsrGraphicsPipelineState *pipelineState) {
  CPSR_ASSUME(pipelineState);

  return (uint8_t)pipelineState->desc.sampleCount;
}

void CpsrGraphicsPipelineStateSetMultisampleCount(CpsrGraphicsPipelineState *pipelineState, uint8_t multisampleCount) {
  CPSR_ASSUME(pipelineState);
  CPSR_ASSUME(multisampleCount > 0);
  CPSR_ASSUME(multisampleCount <= 16);
  CpsrPipelineStateRelease(pipelineState);
  
  pipelineState->desc.sampleCount = multisampleCount;
}

// ---
// Property: Render Target Pixel Format
// ---
void CpsrSetRenderTargetPixelFormat(CpsrGraphicsPipelineState *pipelineState, uint8_t index, CpsrPixelFormat pixelFormat) {
  CPSR_ASSUME(pipelineState);
  CPSR_ASSUME(index < 8);
  CpsrPipelineStateRelease(pipelineState);
  
  pipelineState->desc.colorAttachments[index].pixelFormat = PixelFormatAsMetalType(pixelFormat);
}

void CpsrSetRenderTargetPixelFormatFromTexture2D(CpsrGraphicsPipelineState *pipelineState, uint8_t index, const CpsrTexture2D *renderTarget) {
  CPSR_ASSUME(pipelineState);
  CPSR_ASSUME(index < 8);
  CPSR_ASSUME(renderTarget);
  
  pipelineState->desc.colorAttachments[index].pixelFormat = renderTarget->native.pixelFormat;
}

void CpsrSetRenderTargetPixelFormatFromSwapChain(CpsrGraphicsPipelineState *pipelineState, uint8_t index, CpsrSwapChain *swapChain) {
  CPSR_ASSUME(pipelineState);
  CPSR_ASSUME(index < 8);
  CPSR_ASSUME(swapChain);
  
  pipelineState->desc.colorAttachments[index].pixelFormat = CpsrSwapChainGetNativeTexture(swapChain).pixelFormat;
}
