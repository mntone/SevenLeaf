#include "SnlfModule.h"

struct SnlfColorInputUniforms {
  matrix4x4_t transform;
  float32x4_t color;
};

struct SnlfColorInputContext {
  CpsrGraphicsPipelineState *pipelineState;
  CpsrBuffer *vertexBuffer;
  CpsrBuffer *uniformsBuffer;
};

static intptr_t SnlfColorInputInit(const SnlfGraphicsContext *graphicsContext) {
  const CpsrDevice *device = SnlfGraphicsContextGetDevice(graphicsContext);
  CpsrShaderLibrary *library = CpsrShaderLibraryCreate(device, "./Plugins/libgeneric/");
  CpsrShaderFunction *vertexColorInput = CpsrShaderFunctionCreateFromLibrary(library, "vertexColorInput");
  CpsrGraphicsPipelineState *pipelineState = CpsrGraphicsPipelineStateCreate(device);
  CpsrGraphicsPipelineStateSetPrimitiveTopologyType(pipelineState, CPSR_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
  CpsrGraphicsPipelineStateSetVertexFunction(pipelineState, vertexColorInput);
  CpsrGraphicsPipelineStateSetPixelFunction(pipelineState, SnlfGraphicsContextGetShaderFunction(graphicsContext, SNLF_SHADER_PIXEL_DRAW_COLOR));
  
  float data[] = {
  // x   y
    -1, -1,
    -1,  1,
     1, -1,
     1,  1,
  };
  CpsrBuffer *vertexBuffer = CpsrBufferCreateFromData(device, data, sizeof(data), CPSR_VERTEX_BUFFER);
  if (!vertexBuffer) {
    SnlfOutOfMemoryError();
    return NULL;
  }
  
  CpsrBuffer *uniformsBuffer = CpsrBufferCreateFromSize(device, sizeof(struct SnlfColorInputUniforms), CPSR_HEAP_TYPE_UPLOAD, CPSR_CONSTANT_BUFFER);
  if (!uniformsBuffer) {
    SnlfOutOfMemoryError();
    CpsrBufferDestroy(vertexBuffer);
    return NULL;
  }
  
  struct SnlfColorInputContext *context = (struct SnlfColorInputContext *)malloc(sizeof(struct SnlfColorInputContext));
  if (!context) {
    SnlfOutOfMemoryError();
    CpsrBufferDestroy(vertexBuffer);
    CpsrBufferDestroy(uniformsBuffer);
    return NULL;
  }
  
  context->pipelineState = pipelineState;
  context->vertexBuffer = vertexBuffer;
  context->uniformsBuffer = uniformsBuffer;
  return context;
}

static void SnlfColorInputUninit(intptr_t _context) {
  struct SnlfColorInputContext *context = (struct SnlfColorInputContext *)_context;
  CpsrBufferDestroy(context->vertexBuffer);
  CpsrBufferDestroy(context->uniformsBuffer);
  CpsrGraphicsPipelineStateDestroy(context->pipelineState);
  free(context);
}

static void SnlfColorInputUpdate(intptr_t _context, SnlfGraphicsUpdateParams params) {
  struct SnlfColorInputContext *context = (struct SnlfColorInputContext *)_context;
  
  struct SnlfColorInputUniforms uniforms;
  uniforms.transform = params.world;
  switch ((params.timestamp / 1000000000) % 6) {
  case 0:
    uniforms.color = float32x4_initv(1.F, 0.F, 0.F, 1.F);
    break;

  case 1:
    uniforms.color = float32x4_initv(1.F, 1.F, 0.F, 1.F);
    break;

  case 2:
    uniforms.color = float32x4_initv(0.F, 1.F, 0.F, 1.F);
    break;

  case 3:
    uniforms.color = float32x4_initv(0.F, 1.F, 1.F, 1.F);
    break;

  case 4:
    uniforms.color = float32x4_initv(0.F, 0.F, 1.F, 1.F);
    break;
      
  default:
    uniforms.color = float32x4_initv(1.F, 0.F, 1.F, 1.F);
    break;
  }
  CpsrBufferWrite(context->uniformsBuffer, &uniforms);
}

static void SnlfColorInputDraw(intptr_t _context, SnlfGraphicsDrawParams params) {
  struct SnlfColorInputContext *context = (struct SnlfColorInputContext *)_context;
  
  CpsrSetRenderTargetPixelFormatFromTexture2D(context->pipelineState, 0, params.renderTarget);
  
  CpsrGraphicsContext *graphicsContext = SnlfGraphicsContextCreateGraphicsContext(params.context);
  CpsrGraphicsContextSetRenderTargetFromTexture2D(graphicsContext, params.renderTarget);
  if (!CpsrGraphicsContextSetPipelineState(graphicsContext, context->pipelineState)) {
    CpsrGraphicsContextSetVertexBuffer(graphicsContext, 0, context->vertexBuffer);
    CpsrGraphicsContextSetConstantBuffer(graphicsContext, 0, context->uniformsBuffer);
    CpsrGraphicsContextSetPrimitiveTopology(graphicsContext, CPSR_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP);
    CpsrGraphicsContextDraw(graphicsContext, 0, 6);
    CpsrGraphicsContextClose(graphicsContext);
  }
  CpsrGraphicsContextDestroy(graphicsContext);
}

SnlfGraphicsGenerator colorInputGenerator = {
  .generatorName = "SNLF_GENERIC_COLOR_INPUT",
  .friendlyName  = "Color Input",
  .init          = SnlfColorInputInit,
  .uninit        = SnlfColorInputUninit,
  .update        = SnlfColorInputUpdate,
  .draw          = SnlfColorInputDraw,
};

void SnlfModuleGetInfo(SnlfModuleDescriptor *descriptor) {
  strncpy((char *)descriptor->identifier, "SNLF_GENERIC_UTILS", 64);
  strncpy((char *)descriptor->version,    "0.9.0", 24);
  descriptor->comparableVersion = 1;
  strncpy((char *)descriptor->authorName, "mntone", 32);
}

bool SnlfModuleCanLoad(const SnlfLibraryDescriptor *libraryDescriptor) {
  return libraryDescriptor->compositorVersion == CPSR_API_VERSION
    && libraryDescriptor->sevenleafAPIVersion == SNLF_API_VERSION;
}

intptr_t SnlfModuleLoad(SnlfCoreRef core) {
  return (intptr_t)SnlfGraphicsGeneratorRegister(core, &colorInputGenerator);
}

bool SnlfModuleUnload(intptr_t context) {
  return false;
}
