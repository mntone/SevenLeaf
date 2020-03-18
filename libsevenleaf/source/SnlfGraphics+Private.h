#ifndef _SNLF_GRAPHICS_PRIVATE_H
#define _SNLF_GRAPHICS_PRIVATE_H

#include "SnlfCore+Private.h"

#include <containers/SnlfLockFreeQueue.h>

// ---
// Graphics Data
// ---
typedef struct _SnlfTransitionGraphicsData SnlfTransitionGraphicsData;

#define DEFINE_SNLF_GRAPHICS_COMMON_DATA \
  SnlfLockFreeQueue messageQueue; \
  void (*processMessage)(intptr_t, const SnlfGraphicsContext *); \
  void (*update)(intptr_t, SnlfGraphicsUpdateParams); \
  void (*draw)(intptr_t, SnlfGraphicsDrawParams); \
  void (*uninit)(intptr_t)

#define SnlfGraphicsDataProcessMessage(__DATA__, __CTX__) __DATA__->processMessage(__DATA__, __CTX__);
#define SnlfGraphicsDataUpdate(__DATA__, __PARAMS__)      __DATA__->update(__DATA__, __PARAMS__);

struct _SnlfGraphicsData {
  DEFINE_SNLF_GRAPHICS_COMMON_DATA;
};

inline void SnlfGraphicsDataDraw(SnlfGraphicsData *data, SnlfGraphicsDrawParams params) {
  data->draw(data, params);
}

inline void SnlfGraphicsDataUninit(SnlfGraphicsData *data) {
  data->uninit(data);
}

SnlfTransitionGraphicsData *SnlfTransitionGraphicsDataInit(SnlfCoreRef core, const SnlfGraphicsContext *context);
SnlfGraphicsData *SnlfGraphicsDataInitFromSource(SnlfSourceRef source, const SnlfGraphicsContext *context);

// ---
// Graphics Context
// ---
struct _SnlfGraphicsContext {
  const CpsrDevice *device;
  CpsrCommandQueue *commandQueue;
  CpsrCommandBuffer *commandBuffer;
  
  // Shaders
  CpsrShaderLibrary *shaderDefaultLibrary;
  
  CpsrShaderFunction *drawVS;
  CpsrShaderFunction *drawSimpleVS;
  CpsrShaderFunction *drawHalfPS;
  CpsrShaderFunction *drawSinglePS;
  CpsrGraphicsPipelineState *drawSimplePipelineState;
  
  CpsrShaderFunction *drawColorVS;
  CpsrShaderFunction *drawColorInstancedVS;
  CpsrShaderFunction *drawColorPS;
  
  // Primitive vertex
  CpsrBuffer *planeVertexBuffer;
  
  // Render targets
  size_t renderTargetCurrentIndex;
  CpsrHeap *renderTargetsHeap;
  CpsrTexture2D *renderTargets[SNLF_OUTPUT_BUFFER_COUNT];
};

bool SnlfGraphicsContextInit(SnlfGraphicsContext *context, const CpsrDevice *device, CpsrSizeU32 resolution);
void SnlfGraphicsContextUninit(SnlfGraphicsContext *context);

void SnlfGraphicsContextDrawToSwapChain(const SnlfGraphicsContext *context, const CpsrTexture2D *source, CpsrSwapChain *destination, const CpsrBuffer *transformBuffer);

// ---
// Graphics
// ---
bool SnlfGraphicsInit(SnlfCoreRef core);
void SnlfGraphicsUninit(SnlfCoreRef core);

typedef struct {
  SnlfCoreRef core;
  
  thread_id_t threadId;
  CpsrSizeU32 resolution;
  SnlfFramerateU framerate;
} SnlfGraphicsArgs;

struct _SnlfGraphicsThreadContext {
  SnlfCoreRef core;
  thread_id_t threadId;
  
  uint64_t startTime;
  uint64_t interval;
  uint64_t lastFrameTime;
  uint64_t frameCount, droppedFrameCount;
  SnlfFramerateU framerate;
  
  SnlfGraphicsData *root;
  SnlfGraphicsContext graphics;
};

void *SnlfGraphicsLoop(void *param);

#endif  // _SNLF_GRAPHICS_PRIVATE_H
