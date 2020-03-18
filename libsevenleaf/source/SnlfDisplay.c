#include "SnlfGraphics+Private.h"

#include "SnlfDisplay.h"

#include <errno.h>

// ---
// Prototype
// ---
typedef struct {
  SnlfCoreRef core;
} SnlfDisplayThreadArgs;

struct _SnlfDisplay {
  SnlfCoreRef core;
  CpsrViewHandle handle;
  SnlfDisplayContextDrawHandler handler;
  intptr_t param;
  
  CpsrPixelFormat pixelFormat;
  CpsrSwapChain *swapChain;
  
  bool active;
  bool sizeChanged;
  CpsrSizeU32 size;
};

void *SnlfDisplayLoop(void *param);

#define SnlfDisplayArrayGetAt(array, index) (SnlfDisplay *)SnlfArrayGetPointerAt(array, index)

#define LOCK(__CORE__)   pthread_mutex_lock(&__CORE__->displayMutex)
#define UNLOCK(__CORE__) pthread_mutex_unlock(&__CORE__->displayMutex)

// ---
// Display context
// ---
struct _SnlfDisplayContext {
  SnlfCoreRef core;
  SnlfDisplay *display;
  
  const SnlfGraphicsContext *graphicsContext;
  
  CpsrBuffer *transformBuffer[SNLF_MAX_DISPLAY_COUNT * SNLF_OUTPUT_BUFFER_COUNT];
  CpsrTexture2D *sourceTexture;
};

const SnlfGraphicsContext *SnlfDisplayContextGetGraphicsContext(const SnlfDisplayContext *displayContext) {
  return displayContext->graphicsContext;
}

const CpsrTexture2D *SnlfDisplayContextGetSourceTexture(const SnlfDisplayContext *displayContext) {
  return displayContext->sourceTexture;
}

CpsrSwapChain *SnlfDisplayContextGetSwapChain(const SnlfDisplayContext *displayContext) {
  return displayContext->display->swapChain;
}

// ---
// Init/uninit state in core
// ---
bool SnlfCoreInitForDisplays(SnlfCoreRef core) {
  SnlfArrayInit(core->displays);
  return SnlfRecursiveMutexCreate(&core->displayMutex);
}

bool SnlfCoreUninitForDisplays(SnlfCoreRef core) {
  SnlfArrayRelease(core->displays);
  return SnlfMutexDestroy(&core->displayMutex);
}

// ---
// Register/unregister display to core
// ---
bool SnlfDisplayRegister(SnlfCoreRef core, const SnlfDisplay *display) {
  assert(core);
  assert(display);
  
  if (LOCK(core)) {
    SnlfMutexLockError();
    return true;
  }
  
  if (SnlfArrayAppend(core->displays, display)) {
    return true;
  }
  
  if (UNLOCK(core)) {
    SnlfMutexUnlockError();
  }
  return false;
}

bool SnlfDisplayUnregister(const SnlfDisplay *display) {
  assert(display);
  
  SnlfCoreRef core = display->core;
  
  if (LOCK(core)) {
    SnlfMutexLockError();
    return true;
  }
  
  SnlfArraySizeType index;
  if (SnlfArrayRemove(core->displays, display, &index)) {
    // TODO: Error log
  }
  
  if (UNLOCK(core)) {
    SnlfMutexUnlockError();
  }
  return false;
}

// ---
// Create/Destory
// ---
SnlfDisplay *SnlfDisplayCreate(SnlfCoreRef core, CpsrViewHost viewHost, SnlfDisplayContextDrawHandler handler, intptr_t param) {
  assert(core);
  
  SnlfDisplay *display = SnlfAlloc(SnlfDisplay);
  if (!display) {
    return NULL;
  }
  
  display->core        = core;
  display->handle      = viewHost.handle;
  display->handler     = handler;
  display->param       = param;
  
  display->pixelFormat = viewHost.pixelFormat;
  display->swapChain   = NULL;
  
  display->active      = false;
  display->size        = viewHost.size;
  display->sizeChanged = false;
  
  if (SnlfDisplayRegister(core, display)) {
    SnlfDealloc(display);
    return NULL;
  }
  return display;
}

void SnlfDisplayDestroy(SnlfDisplay *display) {
  assert(display);
  
  SnlfDisplayUnregister(display);
  SnlfDealloc(display);
}

void SnlfDisplayChangeSize(SnlfDisplay *display, CpsrSizeU32 size) {
  assert(display);
  
  if (LOCK(display->core)) {
    SnlfMutexLockError();
    return;
  }
  
  display->size.width = size.width > SNLF_MIN_WIDTH ? size.width : SNLF_MIN_WIDTH;
  display->size.height = size.height > SNLF_MIN_HEIGHT ? size.height : SNLF_MIN_HEIGHT;
  display->sizeChanged = true;
  
  if (UNLOCK(display->core)) {
    SnlfMutexUnlockError();
  }
}

void SnlfDisplayBeginDraw(SnlfDisplay *display) {
  assert(display);
  
  if (LOCK(display->core)) {
    SnlfMutexLockError();
    return;
  }
  
  display->active = true;
  
  if (UNLOCK(display->core)) {
    SnlfMutexUnlockError();
  }
}

void SnlfDisplayEndDraw(SnlfDisplay *display) {
  assert(display);
  
  if (LOCK(display->core)) {
    SnlfMutexLockError();
    return;
  }
  
  display->active = false;
  
  if (UNLOCK(display->core)) {
    SnlfMutexUnlockError();
  }
}

// ---
// Draw
// ---
void SnlfDisplayContextPresent(const SnlfDisplayContext *context) {
  CpsrCommandBuffer *commandBuffer = context->graphicsContext->commandBuffer;
  CpsrSwapChain *swapChain = context->display->swapChain;
  CpsrCommandBufferExecuteAndPresent(commandBuffer, swapChain);
}

// ---
// Display thread
// ---
static inline void SnlfDisplayThreadArgsInit(SnlfDisplayThreadArgs *args, SnlfCoreRef core) {
  args->core = core;
}

static inline bool SnlfDisplayThreadBegin(SnlfCoreRef core) {
  SnlfDisplayThreadArgs *args = SnlfAlloc(SnlfDisplayThreadArgs);
  if (!args) {
    SnlfOutOfMemoryError();
    return true;
  }
  SnlfDisplayThreadArgsInit(args, core);
  
  if (LOCK(core)) {
    SnlfMutexLockError();
    return true;
  }
  
  core->displayThreadActive = true;
  
  int error = pthread_create(&core->displayThread, NULL, SnlfDisplayLoop, (void *)args);
  if (UNLOCK(core)) {
    SnlfMutexUnlockError();
  }
  return error != 0;
}

static inline bool SnlfDisplayThreadFinish(SnlfCoreRef core) {
  int result = 0;
  void *ret;
  
  if (core->displayThreadActive) {
    if (LOCK(core)) {
      SnlfMutexLockError();
      return true;
    }
    
    core->displayThreadActive = false;
    result = pthread_join(core->displayThread, &ret);
    if (result == ESRCH) {
      // Error log
      exit(0);
    } else if (result != 0) {
      // Error log
    }
    
    if (UNLOCK(core)) {
      SnlfMutexUnlockError();
    }
  }
  return false;
}

void SnlfDisplayMain(SnlfDisplayContext *displayContext) {
  SnlfCoreRef core = displayContext->core;
  if (LOCK(core)) {
    SnlfMutexLockError();
    return;
  }
  
  SnlfArrayForeach(core->displays) {
    SnlfDisplay *display = *(SnlfDisplay **)ptr;
    if (!display->active) {
      continue;
    }
    
    if (!display->swapChain) {
      CpsrViewHost viewHost;
      viewHost.handle = display->handle;
      viewHost.pixelFormat = display->pixelFormat;
      viewHost.size = display->size;
      
      CpsrSwapChain *swapChain = CpsrSwapChainCreate(displayContext->graphicsContext->commandQueue, viewHost, 2, false);
      CpsrSwapChainSetColorSpace(swapChain, CPSR_COLORSPACE_BT709);
      
      display->swapChain = swapChain;
      display->sizeChanged = false;
    }
    
    // Handle size change
    if (display->sizeChanged) {
      CpsrSwapChainSetSize(display->swapChain, display->size);
      display->sizeChanged = false;
    }
    
    CpsrSwapChainNextBuffer(display->swapChain);
    
    // Draw callback
    displayContext->display = display;
    display->handler(display->param, displayContext);
  }

  if (UNLOCK(core)) {
    SnlfMutexUnlockError();
  }
}

void SnlfDisplayMainFromGraphicsThreadContext(const SnlfGraphicsThreadContext *graphicsThreadContext) {
  SnlfDisplayContext displayContext;
  displayContext.core = graphicsThreadContext->core;
  displayContext.display = NULL;
  
  const SnlfGraphicsContext *graphicsContext = &graphicsThreadContext->graphics;
  displayContext.graphicsContext = graphicsContext;
  displayContext.sourceTexture = graphicsContext->renderTargets[graphicsContext->renderTargetCurrentIndex];
  
  SnlfDisplayMain(&displayContext);
}

void *SnlfDisplayLoop(void *param) {
  SnlfDisplayThreadArgs *args = (SnlfDisplayThreadArgs *)param;
  SnlfCoreRef core = args->core;
  
  osutil_set_thread_name("Display Thread");
  SnlfVerboseLog("Begin display thread");
 
  SnlfDisplayContext displayContext;
  displayContext.core = core;
  
  while (core->displayThreadActive) {
    SnlfDisplayMain(&displayContext);
  }
  return NULL;
}
