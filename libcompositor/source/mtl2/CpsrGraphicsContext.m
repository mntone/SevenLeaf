#include "CpsrGraphics+Private.h"

// clang-format off
static inline CpsrPrimitiveTopology PrimitiveTopologyAsCpsrType(MTLPrimitiveType primitiveTopology) {
  switch (primitiveTopology) {
  case MTLPrimitiveTypePoint:         return CPSR_PRIMITIVE_TOPOLOGY_POINT;
  case MTLPrimitiveTypeLine:          return CPSR_PRIMITIVE_TOPOLOGY_LINE;
  case MTLPrimitiveTypeLineStrip:     return CPSR_PRIMITIVE_TOPOLOGY_LINE_STRIP;
  case MTLPrimitiveTypeTriangleStrip: return CPSR_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
  case MTLPrimitiveTypeTriangle:
  default:                            return CPSR_PRIMITIVE_TOPOLOGY_TRIANGLE;
  }
}

static inline MTLPrimitiveType PrimitiveTopologyAsMetalType(CpsrPrimitiveTopology primitiveTopology) {
  switch (primitiveTopology) {
  case CPSR_PRIMITIVE_TOPOLOGY_POINT:          return MTLPrimitiveTypePoint;
  case CPSR_PRIMITIVE_TOPOLOGY_LINE:           return MTLPrimitiveTypeLine;
  case CPSR_PRIMITIVE_TOPOLOGY_LINE_STRIP:     return MTLPrimitiveTypeLineStrip;
  case CPSR_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP: return MTLPrimitiveTypeTriangleStrip;
  case CPSR_PRIMITIVE_TOPOLOGY_TRIANGLE:
  default:                                     return MTLPrimitiveTypeTriangle;
  }
}

static inline MTLTriangleFillMode FillModeAsMetalType(CpsrFillMode fillMode) {
  switch (fillMode) {
  case CPSR_FILL_WIREFRAME: return MTLTriangleFillModeLines;
  case CPSR_FILL_SOLID:
  default:                  return MTLTriangleFillModeFill;
  }
}

static inline MTLCullMode CullModeAsMetalType(CpsrCullMode cullMode) {
  switch (cullMode) {
  case CPSR_CULL_NONE:  return MTLCullModeNone;
  case CPSR_CULL_FRONT: return MTLCullModeFront;
  case CPSR_CULL_BACK:
  default:              return MTLCullModeBack;
  }
}

static inline MTLIndexType IndexTypeAsMetalType(CpsrIndexType indexType) {
  switch (indexType) {
  case CPSR_INDEX_TYPE_UINT16: return MTLIndexTypeUInt16;
  case CPSR_INDEX_TYPE_UINT32:
  default:                     return MTLIndexTypeUInt32;
  }
}
// clang-format on

CpsrGraphicsContext *CpsrGraphicsContextCreate(const CpsrCommandBuffer *commandBuffer) {
  CPSR_ASSUME(commandBuffer);
  
  CpsrGraphicsContext *graphicsContext = CpsrAlloc(CpsrGraphicsContext);
  if (graphicsContext) {
    graphicsContext->commandBuffer     = commandBuffer;
    graphicsContext->desc              = [[MTLRenderPassDescriptor alloc] init];
    graphicsContext->indexBuffer       = NULL;
    graphicsContext->indexType         = CPSR_INDEX_TYPE_UINT16;
    graphicsContext->indexBufferOffset = 0;
    graphicsContext->primitiveTopology = MTLPrimitiveTypeTriangle;
    graphicsContext->native = nil;
  }
  return graphicsContext;
}

void CpsrGraphicsContextDestroy(CpsrGraphicsContext *graphicsContext) {
  assert(graphicsContext);
  
  if (graphicsContext->native) {
    assert(graphicsContext->native.retainCount == 1);
    [graphicsContext->native release];
  }
  [graphicsContext->desc release];
  CpsrDealloc(graphicsContext);
}

void CpsrGraphicsContextClearRenderTarget(CpsrGraphicsContext *graphicsContext, uint8_t index, CpsrClearColor color) {
  CPSR_ASSUME(graphicsContext);
  CPSR_ASSUME(!graphicsContext->native);
  CPSR_ASSUME(index < 8); // TODO: iPhone <4
  
  graphicsContext->desc.colorAttachments[0].loadAction = MTLLoadActionClear;
  graphicsContext->desc.colorAttachments[0].clearColor = MTLClearColorMake(color.red, color.green, color.blue, color.alpha);
}

void CpsrGraphicsContextSetRenderTargetFromTexture2D(CpsrGraphicsContext *graphicsContext, const CpsrTexture2D *renderTarget) {
  CPSR_ASSUME(graphicsContext);
  CPSR_ASSUME(!graphicsContext->native);
  CPSR_ASSUME(renderTarget);
  assert(renderTarget->native.usage & MTLTextureUsageRenderTarget);
  
  graphicsContext->desc.colorAttachments[0].texture = renderTarget->native;
}

void CpsrGraphicsContextSetRenderTargetFromSwapChain(CpsrGraphicsContext *graphicsContext, CpsrSwapChain *swapChain) {
  CPSR_ASSUME(graphicsContext);
  CPSR_ASSUME(!graphicsContext->native);
  CPSR_ASSUME(swapChain);
  
  graphicsContext->desc.colorAttachments[0].texture = CpsrSwapChainGetNativeTexture(swapChain);
}

static inline void SetRasterizerState(id<MTLRenderCommandEncoder> commandEncoder, const CpsrRasterizerDescriptor *rasterizerState) {
  CpsrFillMode fillMode = rasterizerState->fillMode;
  if (fillMode != CPSR_FILL_SOLID) {
    [commandEncoder setTriangleFillMode:FillModeAsMetalType(fillMode)];
  }
  
  CpsrCullMode cullMode = rasterizerState->cullMode;
  if (cullMode != CPSR_CULL_NONE) {
    [commandEncoder setCullMode:CullModeAsMetalType(cullMode)];
  }
  
  if (rasterizerState->counterClockwise) {
    [commandEncoder setFrontFacingWinding:MTLWindingCounterClockwise];
  }
  
  if (rasterizerState->depthBias != 0 || rasterizerState->slopeScaledDepthBias > FLT_EPSILON) {
    [commandEncoder setDepthBias:rasterizerState->depthBias slopeScale:rasterizerState->slopeScaledDepthBias clamp:rasterizerState->depthBiasClamp];
  }
  
  CpsrDepthClipMode depthClipMode = rasterizerState->depthClipMode;
  if (depthClipMode != CPSR_DEPTH_CLIP_CLIP) {
    [commandEncoder setDepthClipMode:MTLDepthClipModeClamp];
  }
}

bool CpsrGraphicsContextSetPipelineState(CpsrGraphicsContext *graphicsContext, CpsrGraphicsPipelineState *pipelineState) {
  CPSR_ASSUME(graphicsContext);
  CPSR_ASSUME(!graphicsContext->native);
  CPSR_ASSUME(pipelineState);
  
  id<MTLRenderPipelineState> nativePipelineState = CpsrGraphicsPipelineStateGetNative(pipelineState);
  if (!nativePipelineState) {
    // TODO: log
    return true;
  }
  
  id<MTLRenderCommandEncoder> commandEncoder = [graphicsContext->commandBuffer->native renderCommandEncoderWithDescriptor:graphicsContext->desc];
  
  // Set pipeline state
  [commandEncoder setRenderPipelineState:nativePipelineState];

  // Set rasterizer state
  SetRasterizerState(commandEncoder, &pipelineState->rasterizerDesc);
  
  // Save command encoder
  graphicsContext->native = commandEncoder;
  
  return false;
}

void CpsrGraphicsContextSetViewport(CpsrGraphicsContext *graphicsContext, CpsrViewport viewport) {
  CPSR_ASSUME(graphicsContext);
  CPSR_ASSUME(graphicsContext->native);
  
  MTLViewport native;
  native.originX = viewport.originX;
  native.originY = viewport.originY;
  native.width   = viewport.width;
  native.height  = viewport.height;
  native.znear   = viewport.znear;
  native.zfar    = viewport.zfar;
  [graphicsContext->native setViewport:native];
}

void CpsrGraphicsContextSetScissorRect(CpsrGraphicsContext *graphicsContext, CpsrScissorRect scissorRect) {
  CPSR_ASSUME(graphicsContext);
  CPSR_ASSUME(graphicsContext->native);
  
  MTLScissorRect native;
  native.x      = scissorRect.x;
  native.y      = scissorRect.y;
  native.width  = scissorRect.width;
  native.height = scissorRect.height;
  [graphicsContext->native setScissorRect:native];
}

// ---
// Constant Buffer
// ---
void CpsrGraphicsContextSetConstantBuffer(CpsrGraphicsContext *graphicsContext, uint8_t index, const CpsrBuffer *constantBuffer) {
  CPSR_ASSUME(graphicsContext);
  CPSR_ASSUME(graphicsContext->native);
  CPSR_ASSUME(index < CPSR_CONSTANT_BUFFER_COUNT);
  CPSR_ASSUME(constantBuffer);
  CPSR_ASSUME(constantBuffer->bufferType == CPSR_CONSTANT_BUFFER);
  CPSR_ASSUME(graphicsContext->commandBuffer->commandQueue->device == constantBuffer->device);
  
  [graphicsContext->native setVertexBuffer:constantBuffer->native offset:0 atIndex:CPSR_METAL_CONSTANT_BUFFER_OFFSET + index];
}

void CpsrGraphicsContextSetConstantBufferWithOffset(CpsrGraphicsContext *graphicsContext,
                                                    uint8_t index,
                                                    const CpsrBuffer *constantBuffer,
                                                    uint32_t offset) {
  CPSR_ASSUME(graphicsContext);
  CPSR_ASSUME(graphicsContext->native);
  CPSR_ASSUME(index < CPSR_CONSTANT_BUFFER_COUNT);
#if TARGET_IPHONE_SIMULATOR || TARGET_OS_IPHONE
  CPSR_ASSUME(offset % 16 == 0);
#else
  CPSR_ASSUME(offset % 256 == 0);
#endif
  CPSR_ASSUME(constantBuffer);
  CPSR_ASSUME(constantBuffer->bufferType == CPSR_CONSTANT_BUFFER);
  CPSR_ASSUME(graphicsContext->commandBuffer->commandQueue->device == constantBuffer->device);
  
  [graphicsContext->native setVertexBuffer:constantBuffer->native offset:offset atIndex:CPSR_METAL_CONSTANT_BUFFER_OFFSET + index];
}

// ---
// Vertex Buffer
// ---
void CpsrGraphicsContextSetVertexBuffer(CpsrGraphicsContext *graphicsContext, uint8_t index, const CpsrBuffer *vertexBuffer) {
  CPSR_ASSUME(graphicsContext);
  CPSR_ASSUME(graphicsContext->native);
  CPSR_ASSUME(index < CPSR_VERTEX_BUFFER_COUNT);
  CPSR_ASSUME(vertexBuffer);
  CPSR_ASSUME(vertexBuffer->bufferType == CPSR_VERTEX_BUFFER);
  CPSR_ASSUME(graphicsContext->commandBuffer->commandQueue->device == vertexBuffer->device);
  
  [graphicsContext->native setVertexBuffer:vertexBuffer->native offset:0 atIndex:index];
}

void CpsrGraphicsContextSetVertexBuffers(CpsrGraphicsContext *graphicsContext, CpsrRange range, const CpsrBuffer **vertexBuffers) {
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
  
  id<MTLBuffer> *nativeBuffers = CpsrAlloc(id<MTLBuffer>);
  NSUInteger *offsets = CpsrAlloc(NSUInteger);
  if (nativeBuffers && offsets) {
    NSRange nsRange = NSMakeRange(range.location, range.length);
    for (size_t i = 0; i < range.length; ++i) {
      const CpsrBuffer *vertexBuffer = vertexBuffers[i];
      offsets[i] = 0;
      nativeBuffers[i] = vertexBuffer->native;
    }
    
    [graphicsContext->native setVertexBuffers:nativeBuffers offsets:offsets withRange:nsRange];
    
    CpsrDealloc(offsets);
    CpsrDealloc(nativeBuffers);
  } else {
    // TODO log
    exit(0);
  }
}

// ---
// Index Buffer
// ---
void CpsrGraphicsContextSetIndexBuffer(CpsrGraphicsContext *graphicsContext, const CpsrBuffer *indexBuffer, CpsrIndexType indexType) {
  CPSR_ASSUME(graphicsContext);
  CPSR_ASSUME(graphicsContext->native);
  CPSR_ASSUME(indexBuffer);
  CPSR_ASSUME(indexBuffer->bufferType == CPSR_INDEX_BUFFER);
  CPSR_ASSUME(graphicsContext->commandBuffer->commandQueue->device == indexBuffer->device);
  
  graphicsContext->indexBuffer       = indexBuffer;
  graphicsContext->indexType         = IndexTypeAsMetalType(indexType);
  graphicsContext->indexBufferOffset = 0;
}

void CpsrGraphicsContextSetIndexBufferWithOffset(CpsrGraphicsContext *graphicsContext, const CpsrBuffer *indexBuffer, uint32_t offset, CpsrIndexType indexType) {
  CPSR_ASSUME(graphicsContext);
  CPSR_ASSUME(graphicsContext->native);
  CPSR_ASSUME(indexBuffer);
  CPSR_ASSUME(indexBuffer->bufferType == CPSR_INDEX_BUFFER);
#if TARGET_IPHONE_SIMULATOR || TARGET_OS_IPHONE
  CPSR_ASSUME(offset % 16 == 0);
#else
  CPSR_ASSUME(offset % 256 == 0);
#endif
  CPSR_ASSUME(graphicsContext->commandBuffer->commandQueue->device == indexBuffer->device);
  
  graphicsContext->indexBuffer       = indexBuffer;
  graphicsContext->indexType         = IndexTypeAsMetalType(indexType);
  graphicsContext->indexBufferOffset = offset;
}

// ---
// Texture
// ---
void CpsrGraphicsContextSetTexture(const CpsrGraphicsContext *graphicsContext, uint8_t index, const CpsrTexture2D *texture) {
  CPSR_ASSUME(graphicsContext);
  CPSR_ASSUME(graphicsContext->native);
  CPSR_ASSUME(texture);
  CPSR_ASSUME(graphicsContext->commandBuffer->commandQueue->device == texture->device);
  
  [graphicsContext->native setFragmentTexture:texture->native atIndex:0];
}

// ---
// Draw
// ---
void CpsrGraphicsContextSetPrimitiveTopology(CpsrGraphicsContext *graphicsContext,
                                                    CpsrPrimitiveTopology primitiveTopology) {
  CPSR_ASSUME(graphicsContext);

  MTLPrimitiveType metalPrimitiveTopology = PrimitiveTopologyAsMetalType(primitiveTopology);
  graphicsContext->primitiveTopology = metalPrimitiveTopology;
}

void CpsrGraphicsContextDraw(const CpsrGraphicsContext *graphicsContext, uint32_t vertexStart, uint32_t vertexCount) {
  CPSR_ASSUME(graphicsContext);
  CPSR_ASSUME(graphicsContext->native);
  
  [graphicsContext->native drawPrimitives:graphicsContext->primitiveTopology vertexStart:vertexStart vertexCount:vertexCount];
}

void CpsrGraphicsContextDrawInstanced(const CpsrGraphicsContext *graphicsContext, uint32_t vertexStart, uint32_t vertexCount, uint32_t instanceCount) {
  CPSR_ASSUME(graphicsContext);
  CPSR_ASSUME(graphicsContext->native);
  
  [graphicsContext->native drawPrimitives:graphicsContext->primitiveTopology vertexStart:vertexStart vertexCount:vertexCount instanceCount:instanceCount];
}

void CpsrGraphicsContextDrawIndexed(const CpsrGraphicsContext *graphicsContext, uint32_t indexCount) {
  CPSR_ASSUME(graphicsContext);
  CPSR_ASSUME(graphicsContext->native);
  
  [graphicsContext->native drawIndexedPrimitives:graphicsContext->primitiveTopology indexCount:indexCount indexType:graphicsContext->indexType indexBuffer:graphicsContext->indexBuffer->native indexBufferOffset:graphicsContext->indexBufferOffset];
}

void CpsrGraphicsContextDrawIndexedInstanced(const CpsrGraphicsContext *graphicsContext, uint32_t indexCount, uint32_t instanceCount, int32_t baseVertex, uint32_t baseInstance) {
  CPSR_ASSUME(graphicsContext);
  CPSR_ASSUME(graphicsContext->native);
  
  [graphicsContext->native drawIndexedPrimitives:graphicsContext->primitiveTopology indexCount:indexCount indexType:graphicsContext->indexType indexBuffer:graphicsContext->indexBuffer->native indexBufferOffset:graphicsContext->indexBufferOffset instanceCount:instanceCount baseVertex:baseVertex baseInstance:baseInstance];
}

// ---
// End encoding
// ---
void CpsrGraphicsContextClose(const CpsrGraphicsContext *graphicsContext) {
  CPSR_ASSUME(graphicsContext);
  CPSR_ASSUME(graphicsContext->native);
  
  [graphicsContext->native endEncoding];
}

// ---
// Debug
// ---
#ifndef NDEBUG
void _CpsrGraphicsContextPushDebugGroup(const CpsrGraphicsContext *graphicsContext, const char *groupName) {
  CPSR_ASSUME(graphicsContext);
  CPSR_ASSUME(groupName);
  
  NSString *nsGroupName = [[NSString alloc] initWithBytesNoCopy:(void *)groupName length:strlen(groupName) encoding:NSUTF8StringEncoding freeWhenDone:NO];
  [graphicsContext->native pushDebugGroup:nsGroupName];
  [nsGroupName release];
}

void _CpsrGraphicsContextPopDebugGroup(const CpsrGraphicsContext *graphicsContext) {
  CPSR_ASSUME(graphicsContext);
  
  [graphicsContext->native popDebugGroup];
}
#endif
