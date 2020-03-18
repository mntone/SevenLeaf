#ifndef _SNLF_CORE_PRIVATE_H
#define _SNLF_CORE_PRIVATE_H

#include "SnlfLog.h"
#include "SnlfCore.h"
#include "SnlfMessage.h"
#include "SnlfUtils+Private.h"

#include "containers/SnlfArray.h"
#include "containers/SnlfLockFreeQueue+Prototypes.h"

#include <stdlib.h>
#include <osutil_atomic.h>
#include <osutil_dll.h>

#define SnlfAlloc(__TYPE__) (__TYPE__ *)malloc(sizeof(__TYPE__))
#define SnlfAllocRef(__TYPE__) (__TYPE__##Ref)malloc(sizeof(struct _##__TYPE__))
#define SnlfDealloc(__OBJ__) free((void *)__OBJ__)

#ifndef NDEBUG
#if defined(__clang__)
#define SnlfAssume(__COND__) __builtin_assume(__COND__)
#elif defined(__GNUC__) || defined(__GNUG__)
#define SnlfAssume(__COND__) if (!(__COND__)) __builtin_unreachable()
#elif defined(_MSC_VER)
#define SnlfAssume(__COND__) __assume(__COND__)
#else
#define SnlfAssume(__COND__) assert(__COND__)
#endif
#else
#define SnlfAssume(__COND__) assert(__COND__)
#endif

#ifdef __cplusplus
extern "C" {
#endif

// ---
// Prototypes
// ---
typedef uint8_t thread_id_t;

typedef struct _SnlfGraphicsData SnlfGraphicsData;

// ---
// Transition change notification support
// ---
typedef struct {
  SnlfSourceChangedHandler handler;
  intptr_t param;
} SnlfSourceChangedBag;

// ---
// Array change notification support
// ---
typedef struct _SnlfArrayChangedBag {
  SnlfArrayRef array;
  size_t itemSize;
  SnlfArrayChangedHandler handler;
  intptr_t param;
} SnlfArrayChangedBag;

// ---
// Core
// ---
struct _SnlfCore {
  SNLF_ARRAY(SnlfModuleRef) modules;
  
  // Graphics
  CpsrDevice *device;
  pthread_mutex_t videoThreadMutex;
  bool videoActive;
  pthread_t videoThread;
  SnlfGraphicsThreadContext *graphicsThreadContext;
  
  // Generators
  SNLF_ARRAY(SnlfGraphicsGeneratorRef) graphicsGenerators;
  
  // Inputs
  pthread_mutex_t inputMutex;
  identifier_t inputUniqueIdentifier;
  SNLF_ARRAY(SnlfInputRef) inputs;
  SNLF_ARRAY(SnlfArrayChangedBag *) inputHandlers;
  
  // Transision
  pthread_mutex_t transitionMutex;
  SnlfSourceRef currentSource;
  SNLF_ARRAY(SnlfSourceChangedBag *) transitionHandlers;
  
  // Sources
  pthread_mutex_t sourceMutex;
  SNLF_ARRAY(SnlfSourceRef) sources;
  SNLF_ARRAY(SnlfArrayChangedBag *) sourceHandlers;
  
  // Displays
  pthread_mutex_t displayMutex;
  SNLF_ARRAY(SnlfDisplay *) displays;
  bool displayThreadActive;
  pthread_t displayThread;
};

bool SnlfCoreInitForGenerators(SnlfCoreRef core);
bool SnlfCoreUninitForGenerators(SnlfCoreRef core);
bool SnlfCoreInitForInputs(SnlfCoreRef core);
bool SnlfCoreUninitForInputs(SnlfCoreRef core);
bool SnlfCoreInitForTransition(SnlfCoreRef core);
bool SnlfCoreUninitForTransition(SnlfCoreRef core);
bool SnlfCoreInitForSources(SnlfCoreRef core);
bool SnlfCoreUninitForSources(SnlfCoreRef core);
bool SnlfCoreInitForDisplays(SnlfCoreRef core);
bool SnlfCoreUninitForDisplays(SnlfCoreRef core);

// ---
// Object
// ---
#define DEFINE_SNLF_OBJECT_COMMON_DATA \
  SnlfCoreRef           core; \
  osutil_atomic_int32_t refCount

struct _SnlfObject {
  DEFINE_SNLF_OBJECT_COMMON_DATA;
};

// ---
// Input
// ---
struct _SnlfInput {
  DEFINE_SNLF_OBJECT_COMMON_DATA;
  osutil_atomic_int32_t activeCount;
  identifier_t          identifier;
  SnlfInputDescriptor   descriptor;
  intptr_t              context;
};

#define SnlfInputArrayGetAt(array, index) *(SnlfInputRef *)SnlfArrayGetPointerAt(array, index)

// ---
// Source
// ---
typedef enum {
  SNLF_SOURCE_EMPTY,
  SNLF_SOURCE_GROUP,              // SnlfSourceRef[]
  SNLF_SOURCE_REFERENCE,          // SnlfSourceRef
  SNLF_SOURCE_INPUT,              // SnlfInputRef
  SNLF_SOURCE_GRAPHICS_GENERATOR, // SnlfGraphicsGeneratorRef
  SNLF_SOURCE_SOUND_GENERATOR,    // SnlfSoundGeneratorRef
} SnlfSourceType;

struct _SnlfSource {
  DEFINE_SNLF_OBJECT_COMMON_DATA;
  identifier_t identifier;
  SNLF_ARRAY(SnlfArrayChangedBag *) handlers;
  
  // Flags
  SnlfSourceType type : 3;
  bool enabled        : 1;
  bool interaction    : 1; // Process event or do not
  int  _reserved      : 3;
  
  // Relations
  SnlfSourceRef               parent;
  union {
    SNLF_ARRAY(SnlfSourceRef) children;
    SnlfSourceRef             reference;
    SnlfInputRef              input;
    SnlfGraphicsGeneratorRef  graphicsGenerator;
    SnlfSoundGeneratorRef     soundGenerator;
  };
  
  // Graphics
  SnlfBoundsType boundsType;
  matrix4x4_t transform;
  SNLF_ARRAY(SnlfGraphicsData *) graphicsData;
  SNLF_ARRAY(SnlfGraphicsTransformer *) backdropTransformers;
  SNLF_ARRAY(SnlfGraphicsTransformer *) userTransformers;
  
  // Sounds
  
  // Interaction
  void (*click)(bool *);
  void (*pointerDown)(bool *);
  void (*pointerUp)(bool *);
  void (*pointerEnter)(bool *);
  void (*pointerLeave)(bool *);
  void (*wheelMove)(bool *);
};

#define SnlfSourceArrayGetAt(array, index) *(SnlfSourceRef *)SnlfArrayGetPointerAt(array, index)

// ---
// Convenient macro
// ---
#define _SNLF_IMPLEMENTS_REFCOUNT_BASE(__NAME__) \
  extern inline int32_t Snlf##__NAME__##AddRef(Snlf##__NAME__##Ref obj); \
  extern inline int32_t Snlf##__NAME__##GetRefCount(Snlf##__NAME__##Ref obj)

#define SNLF_IMPLEMENTS_REFCOUNT(__NAME__) \
  _SNLF_IMPLEMENTS_REFCOUNT_BASE(__NAME__); \
  extern inline int32_t Snlf##__NAME__##Release(Snlf##__NAME__##Ref obj);

#define SNLF_IMPLEMENTS_REFCOUNT_CUSTOM(__NAME__) \
  _SNLF_IMPLEMENTS_REFCOUNT_BASE(__NAME__); \
  \
  int32_t Snlf##__NAME__##Release(Snlf##__NAME__##Ref obj) { \
    int32_t ret = osutil_atomic_fetch_decrement32(&obj->refCount); \
    SnlfVerboseLogFormat("Release reference count: %d on %p (Snlf%s)", ret - 1, obj, #__NAME__); \
    if (ret == 1) { \
      Snlf##__NAME__##Destroy(obj);\
    } \
    return --ret; \
  }

// ---
// Clean up
// ---
#undef DEFINE_SNLF_OBJECT_COMMON_DATA

#ifdef __cplusplus
}
#endif

#endif  // _SNLF_CORE_PRIVATE_H
