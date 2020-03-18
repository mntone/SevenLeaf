#include <compositor/CpsrUtils.h>
#include <errno.h>

#include "SnlfCore+Private.h"
#include "SnlfGraphics+Private.h"

#include "osutil_uint128_t.h"

extern inline void SnlfGraphicsDataDraw(SnlfGraphicsData *data, SnlfGraphicsDrawParams params);
extern inline void SnlfGraphicsDataUninit(SnlfGraphicsData *data);

static inline void GraphicsThreadCleanUp(SnlfGraphicsArgs *args) {
  SnlfDealloc(args);
}

static inline SnlfGraphicsArgs *SnlfGraphicsThreadStateCreate(SnlfCoreRef core) {
  // Create graphics state
  SnlfGraphicsArgs *args = SnlfAlloc(SnlfGraphicsArgs);
  if (!args) {
    SnlfOutOfMemoryError();
    return NULL;
  }

  // Save graphics state
  args->core = core;
  args->threadId = 0;
  args->resolution.width = 1280;
  args->resolution.height = 720;
  args->framerate.numerator = 60000;
  args->framerate.denominator = 1001;
  return args;
}

static inline bool SnlfGraphicsThreadBegin(SnlfCoreRef core) {
  SnlfGraphicsArgs *args = SnlfGraphicsThreadStateCreate(core);
  if (!args) {
    return true;
  }

  if (pthread_mutex_lock(&core->videoThreadMutex) != 0) {
    return true;
  }

  core->videoActive = true;

  int error = pthread_create(&core->videoThread, NULL, SnlfGraphicsLoop, (void *)args);
  if (pthread_mutex_unlock(&core->videoThreadMutex) != 0) {
    // Error log
  }
  return error != 0;
}

static inline bool SnlfGraphicsThreadFinish(SnlfCoreRef core) {
  int result = 0;
  void *ret;

  if (core->videoActive) {
    if (pthread_mutex_lock(&core->videoThreadMutex) != 0) {
      return true;
    }

    core->videoActive = false;
    result = pthread_join(core->videoThread, &ret);
    if (result == ESRCH) {
      // Error log
      exit(0);
    } else if (result != 0) {
      // Error log
    }

    if (pthread_mutex_unlock(&core->videoThreadMutex) != 0) {
      // Error log
    }
  }
  return false;
}

bool SnlfGraphicsInit(SnlfCoreRef core) {
  pthread_mutex_t videoThreadMutex = PTHREAD_RECURSIVE_MUTEX_INITIALIZER;

  if (pthread_mutex_init(&videoThreadMutex, NULL) < 0) {
    return true;
  }

  CpsrDevice *device = CpsrDeviceGetDefault();
  if (!device) {
    pthread_mutex_destroy(&videoThreadMutex);
    return true;
  }

  core->device = device;
  core->videoThreadMutex = videoThreadMutex;
  core->graphicsThreadContext = NULL;
  return SnlfGraphicsThreadBegin(core);
}

void SnlfGraphicsUninit(SnlfCoreRef core) {
  assert(core);

  SnlfGraphicsThreadFinish(core);
  CpsrDeviceDestroy(core->device);
  pthread_mutex_destroy(&core->videoThreadMutex);
  SnlfDealloc(core);
}

// ---
// Graphics thread uninit
// ---
static inline void SnlfGraphicsThreadUninit(SnlfGraphicsThreadContext *graphicsThreadContext) {
  // TODO: release rootGraphicsData

  SnlfGraphicsContextUninit(&graphicsThreadContext->graphics);
  SnlfDealloc(graphicsThreadContext);
}

// ---
// Graphics thread init
// ---
static inline SnlfGraphicsThreadContext *SnlfGraphicsThreadInit(SnlfGraphicsArgs *args) {
  const CpsrDevice *device = args->core->device;

  SnlfGraphicsThreadContext *graphicsThreadContext = SnlfAlloc(SnlfGraphicsThreadContext);
  if (!graphicsThreadContext) {
    SnlfOutOfMemoryError();
    return NULL;
  }
  memset(graphicsThreadContext, 0, sizeof(SnlfGraphicsThreadContext));

  graphicsThreadContext->core = args->core;
  graphicsThreadContext->threadId = args->threadId;

  graphicsThreadContext->framerate = args->framerate;
  graphicsThreadContext->interval =
      osutil_udiv128(osutil_umul64x64(1000000000, args->framerate.denominator), args->framerate.numerator);

  graphicsThreadContext->graphics.device = device;

  // Initialize graphics context
  if (SnlfGraphicsContextInit(&graphicsThreadContext->graphics, device, args->resolution)) {
    SnlfGraphicsThreadUninit(graphicsThreadContext);
    return NULL;
  }

  // Initialize transition
  SnlfTransitionGraphicsData *rootGraphicsData = SnlfTransitionGraphicsDataInit(args->core, &graphicsThreadContext->graphics);
  if (!rootGraphicsData) {
    SnlfGraphicsContextUninit(&graphicsThreadContext->graphics);
    SnlfGraphicsThreadUninit(graphicsThreadContext);
    return NULL;
  }
  graphicsThreadContext->root = (SnlfGraphicsData *)rootGraphicsData;

  return graphicsThreadContext;
}

// ---
// Graphics thread sleep
// ---
static inline void SnlfGraphicsThreadSleep(SnlfGraphicsThreadContext *graphicsThreadContext) {
  uint64_t interval = graphicsThreadContext->interval;
  uint64_t lastFrameTime = graphicsThreadContext->lastFrameTime;
  uint64_t currentFrameTime = lastFrameTime + interval;
  if (!osutil_wait_until_nanoseconds(currentFrameTime)) {
    graphicsThreadContext->frameCount += 1;
    graphicsThreadContext->lastFrameTime = currentFrameTime;
  } else {
    uint64_t frameCount = (osutil_gettime_as_nanoseconds() - lastFrameTime) / interval;
    graphicsThreadContext->frameCount += frameCount;
    graphicsThreadContext->droppedFrameCount += frameCount - 1;
    graphicsThreadContext->lastFrameTime = lastFrameTime + interval * frameCount;
  }
}

// ---
// Graphics thread main loop
// ---
void SnlfDisplayMainFromGraphicsThreadContext(const SnlfGraphicsThreadContext *graphicsThreadContext);

void *SnlfGraphicsLoop(void *param) {
  SnlfGraphicsArgs *args = (SnlfGraphicsArgs *)param;
  SnlfCoreRef core = args->core;

  osutil_set_thread_name("Graphics Thread");
  CpsrSetCurrentThreadPriority(CSPR_TP_GRAPHICS);
  SnlfVerboseLogFormat("Begin graphics thread #%d", args->threadId);

  SnlfGraphicsThreadContext *graphicsThreadContext = SnlfGraphicsThreadInit(args);
  if (!graphicsThreadContext) {
    goto cleanup;
  }
  core->graphicsThreadContext = graphicsThreadContext;

  graphicsThreadContext->startTime = osutil_gettime_as_nanoseconds();
  graphicsThreadContext->lastFrameTime = graphicsThreadContext->startTime;

  while (core->videoActive) {
    // Process commands
    SnlfGraphicsDataProcessMessage(graphicsThreadContext->root, &graphicsThreadContext->graphics);

    // Trace appropriate timestamp
    SnlfGraphicsUpdateParams updateParams;
    updateParams.timestamp = graphicsThreadContext->lastFrameTime + graphicsThreadContext->interval;
    updateParams.world = matrix4x4_idt();
    SnlfGraphicsDataUpdate(graphicsThreadContext->root, updateParams);

    // Create current command queue
    CpsrCommandBuffer *commandBuffer = CpsrCommandBufferCreate(graphicsThreadContext->graphics.commandQueue);
    graphicsThreadContext->graphics.commandBuffer = commandBuffer;

    // Draw main content
    if (++graphicsThreadContext->graphics.renderTargetCurrentIndex == SNLF_OUTPUT_BUFFER_COUNT) {
      graphicsThreadContext->graphics.renderTargetCurrentIndex = 0;
    }

    SnlfGraphicsDrawParams drawParams;
    drawParams.context = &graphicsThreadContext->graphics;
    drawParams.renderTarget =
        graphicsThreadContext->graphics.renderTargets[graphicsThreadContext->graphics.renderTargetCurrentIndex];
    SnlfGraphicsDataDraw(graphicsThreadContext->root, drawParams);
    CpsrCommandBufferExecute(graphicsThreadContext->graphics.commandBuffer);

    // Notify display thread
    SnlfDisplayMainFromGraphicsThreadContext(graphicsThreadContext);

    // Clean up command queue
    CpsrCommandBufferDestroy(commandBuffer);
    graphicsThreadContext->graphics.commandBuffer = NULL;

    // Sleep graphics thread
    SnlfGraphicsThreadSleep(graphicsThreadContext);
  }

  SnlfVerboseLogFormat("End graphics thread #%d", graphicsThreadContext->threadId);

  SnlfGraphicsThreadUninit(graphicsThreadContext);
cleanup:
  GraphicsThreadCleanUp(args);
  return NULL;
}
