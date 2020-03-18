#include "SnlfGraphics+Private.h"

// ---
// Property
// ---
const CpsrDevice *SnlfGraphicsContextGetDevice(const SnlfGraphicsContext *context) {
  return context->device;
}

const CpsrCommandBuffer *SnlfGraphicsContextGetCommandBuffer(const SnlfGraphicsContext *context) {
  return context->commandBuffer;
}

CpsrGraphicsContext *SnlfGraphicsContextCreateGraphicsContext(const SnlfGraphicsContext *context) {
  return CpsrGraphicsContextCreate(context->commandBuffer);
}

const CpsrShaderFunction *SnlfGraphicsContextGetShaderFunction(const SnlfGraphicsContext *context, SnlfShaderFunction function) {
  switch (function) {
  case SNLF_SHADER_VERTEX_DRAW:                 return context->drawVS;
  case SNLF_SHADER_VERTEX_DRAW_SIMPLE:          return context->drawSimpleVS;
  case SNLF_SHADER_PIXEL_DRAW_HALF:             return context->drawHalfPS;
  case SNLF_SHADER_PIXEL_DRAW_SINGLE:           return context->drawSinglePS;
  case SNLF_SHADER_VERTEX_DRAW_COLOR:           return context->drawColorVS;
  case SNLF_SHADER_VERTEX_DRAW_COLOR_INSTANCED: return context->drawColorInstancedVS;
  case SNLF_SHADER_PIXEL_DRAW_COLOR:            return context->drawColorPS;
  default:
    return NULL;
  }
}

// ---
// Init/uninit state
// ---
bool SnlfGraphicsContextInit(SnlfGraphicsContext *context, const CpsrDevice *device, CpsrSizeU32 resolution) {
  // Initialize graphics resources
  CpsrShaderLibrary *library = CpsrShaderLibraryCreate(device, "./Resources/");
  if (!library) {
    SnlfErrorLog("Create shader default library failed.");
    SnlfGraphicsContextUninit(context);
    return true;
  }
  context->shaderDefaultLibrary = library;
  
#define INIT_SHADER_FUNCTION(__NAME__, __FUNCTION_NAME__) \
  CpsrShaderFunction *__NAME__ = CpsrShaderFunctionCreateFromLibrary(library, #__FUNCTION_NAME__); \
  if (!__NAME__) { \
    SnlfErrorLogFormat("Create shader function \"%s\" failed.", #__FUNCTION_NAME__); \
    SnlfGraphicsContextUninit(context); \
    return NULL; \
  } \
  context->__NAME__ = __NAME__
  
#define INIT_PIPELINE_STATE(__NAME__, __VERTEX_FN__, __PIXEL_FN__) \
  CpsrGraphicsPipelineState *__NAME__ = CpsrGraphicsPipelineStateCreate(device); \
  if (!__NAME__) { \
    SnlfErrorLogFormat("Create pipeline state \"%s\" failed.", #__NAME__); \
    SnlfGraphicsContextUninit(context); \
    return NULL;\
  } \
  CpsrGraphicsPipelineStateSetPrimitiveTopologyType(__NAME__, CPSR_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE); \
  CpsrGraphicsPipelineStateSetVertexFunction(__NAME__, __VERTEX_FN__); \
  CpsrGraphicsPipelineStateSetPixelFunction(__NAME__, __PIXEL_FN__); \
  context->__NAME__ = __NAME__
  
  INIT_SHADER_FUNCTION(drawVS, DrawVS);
  INIT_SHADER_FUNCTION(drawSimpleVS, DrawSimpleVS);
  INIT_SHADER_FUNCTION(drawHalfPS, DrawHalfPS);
  INIT_SHADER_FUNCTION(drawSinglePS, DrawSinglePS);
  INIT_PIPELINE_STATE(drawSimplePipelineState, drawSimpleVS, drawHalfPS);
  
  INIT_SHADER_FUNCTION(drawColorVS, DrawColorVS);
  INIT_SHADER_FUNCTION(drawColorInstancedVS, DrawColorInstancedVS);
  INIT_SHADER_FUNCTION(drawColorPS, DrawColorPS);

#undef INIT_PIPELINE_STATE
#undef INIT_SHADER_FUNCTION
  
  float planeVertex[] = {
    -1, -1,
    -1,  1,
     1, -1,
     1,  1,
  };
  CpsrBuffer *planeVertexBuffer = CpsrBufferCreateFromData(device, planeVertex, sizeof(planeVertex), CPSR_VERTEX_BUFFER);
  if (!planeVertexBuffer) {
    SnlfAssertWithOutOfMemory("Create plane vertex buffer failed");
    SnlfGraphicsContextUninit(context);
    return true;
  }
  context->planeVertexBuffer = planeVertexBuffer;
  
  // Initialize render target
  CpsrTexture2DDescriptor desc;
  desc.size = resolution;
  desc.arrayLength = 1;
  desc.pixelFormat = CPSR_PIXELFORMAT_RGBA16_FLOAT;
  desc.usage = CPSR_TEXTURE_USAGE_READ | CPSR_TEXTURE_USAGE_RENDER_TARGET;
  CpsrHeap *renderTargetsHeap = CpsrHeapCreateFormTexture2DDescriptor(device, &desc, SNLF_OUTPUT_BUFFER_COUNT, CPSR_HEAP_TYPE_DEFAULT);
  if (!renderTargetsHeap) {
    SnlfAssertWithOutOfMemory("Create heap of render targets failed");
    SnlfGraphicsContextUninit(context);
    return true;
  }
  context->renderTargetsHeap = renderTargetsHeap;
  
  for (size_t i = 0; i < SNLF_OUTPUT_BUFFER_COUNT; ++i) {
    CpsrTexture2D *renderTarget = CpsrTexture2DCreateFromHeap(renderTargetsHeap, &desc);
    if (!renderTarget) {
      SnlfAssertWithOutOfMemory("Create texture failed");
      SnlfGraphicsContextUninit(context);
      return true;
    }
    context->renderTargets[i] = renderTarget;
  }
  
  // Initialize command queue
  CpsrCommandQueue *commandQueue = CpsrCommandQueueCreate(device);
  if (!commandQueue) {
    SnlfAssertWithOutOfMemory("Create command allocator failed");
    SnlfGraphicsContextUninit(context);
    return true;
  }
  context->commandQueue = commandQueue;
  
  return false;
}

void SnlfGraphicsContextUninit(SnlfGraphicsContext *context) {
  if (context->commandQueue) {
    CpsrCommandQueueDestroy(context->commandQueue);
    context->commandQueue = NULL;
  }
  if (context->planeVertexBuffer) {
    CpsrBufferDestroy(context->planeVertexBuffer);
    context->planeVertexBuffer = NULL;
  }

  for (size_t i = 0; i < SNLF_OUTPUT_BUFFER_COUNT; ++i) {
    CpsrTexture2D *renderTarget = context->renderTargets[i];
    if (renderTarget) {
      CpsrTexture2DDestroy(renderTarget);
      context->renderTargets[i] = NULL;
    }
  }
  if (context->renderTargetsHeap) {
    CpsrHeapDestroy(context->renderTargetsHeap);
    context->renderTargetsHeap = NULL;
  }
  
#define RELEASE_SHADER_FUNCTION(__NAME__) \
  if (context->__NAME__) { \
    CpsrShaderFunctionDestroy(context->__NAME__); \
    context->__NAME__ = NULL; \
  }
  
#define RELEASE_PIPELINE_STATE(__NAME__) \
  if (context->__NAME__) { \
    CpsrGraphicsPipelineStateDestroy(context->__NAME__); \
    context->__NAME__ = NULL; \
  }
  
  RELEASE_PIPELINE_STATE(drawSimplePipelineState);
  RELEASE_SHADER_FUNCTION(drawVS);
  RELEASE_SHADER_FUNCTION(drawSimpleVS);
  RELEASE_SHADER_FUNCTION(drawHalfPS);
  RELEASE_SHADER_FUNCTION(drawSinglePS);
  
  RELEASE_SHADER_FUNCTION(drawColorVS);
  RELEASE_SHADER_FUNCTION(drawColorInstancedVS);
  RELEASE_SHADER_FUNCTION(drawColorPS);

#undef RELEASE_PIPELINE_STATE
#undef RELEASE_SHADER_FUNCTION
  
  if (context->shaderDefaultLibrary) {
    CpsrShaderLibraryDestroy(context->shaderDefaultLibrary);
    context->shaderDefaultLibrary = NULL;
  }
}

// ---
// Draw primitive
// ---
void SnlfGraphicsContextDrawToSwapChain(const SnlfGraphicsContext *context, const CpsrTexture2D *source, CpsrSwapChain *destination, const CpsrBuffer *transformBuffer) {
  CpsrGraphicsPipelineState *pipelineState = context->drawSimplePipelineState;
  CpsrSetRenderTargetPixelFormatFromSwapChain(pipelineState, 0, destination);
  
  CpsrCommandBuffer *commandBuffer = context->commandBuffer;
  CpsrGraphicsContext *graphicsContext = CpsrGraphicsContextCreate(commandBuffer);
  CpsrGraphicsContextSetRenderTargetFromSwapChain(graphicsContext, destination);
  if (!CpsrGraphicsContextSetPipelineState(graphicsContext, pipelineState)) {
    CpsrGraphicsContextSetVertexBuffer(graphicsContext, 0, context->planeVertexBuffer);
    CpsrGraphicsContextSetConstantBuffer(graphicsContext, 0, transformBuffer);
    CpsrGraphicsContextSetTexture(graphicsContext, 0, source);
    CpsrGraphicsContextSetPrimitiveTopology(graphicsContext, CPSR_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP);
    CpsrGraphicsContextDraw(graphicsContext, 0, 4);
    CpsrGraphicsContextClose(graphicsContext);
  }
  CpsrGraphicsContextDestroy(graphicsContext);
}
