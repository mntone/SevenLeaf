#ifndef _SNLF_CORE_H
#define _SNLF_CORE_H

#include <osutil.h>

#include <compositor/CpsrGraphics.h>
#include <compositor/vector/matrix4x4_t.h>

#include "SnlfConfig.h"
#include "SnlfLog.h"
#include "SnlfPrimitives.h"

#ifdef __cplusplus
extern "C" {
#endif

// ---
// Prototypes
// ---
typedef uint32_t identifier_t;

// Time
typedef uint64_t timestamp_t;
typedef uint64_t duration_t;
typedef float normalized_time_t;

// Descriptors
typedef struct _SnlfInputDescriptor SnlfInputDescriptor;
typedef struct _SnlfOutputDescriptor SnlfOutputDescriptor;

// Core type
typedef struct _SnlfDisplay SnlfDisplay;

typedef struct _SnlfCore *SnlfCoreRef;
typedef struct _SnlfModule *SnlfModuleRef;
typedef struct _SnlfObject *SnlfObjectRef;
typedef struct _SnlfInput *SnlfInputRef;
typedef struct _SnlfSource *SnlfSourceRef;

typedef struct _SnlfOutput SnlfOutput;

typedef struct _SnlfGraphicsContext SnlfGraphicsContext;
typedef struct _SnlfGraphicsThreadContext SnlfGraphicsThreadContext;
typedef struct _SnlfGraphicsGenerator SnlfGraphicsGenerator;
typedef const struct _SnlfGraphicsGenerator *SnlfGraphicsGeneratorRef;
typedef struct _SnlfGraphicsTransformer SnlfGraphicsTransformer;

typedef struct _SnlfSoundContext SnlfSoundContext;
typedef struct _SnlfSoundThreadContext SnlfSoundThreadContext;
typedef struct _SnlfSoundGenerator SnlfSoundGenerator;
typedef const struct _SnlfSoundGenerator *SnlfSoundGeneratorRef;
typedef struct _SnlfSoundTransformer SnlfSoundTransformer;

typedef struct {
  uint32_t numerator;
  uint32_t denominator;
} SnlfFramerateU;

typedef struct {
  uint32_t location, length;
} SnlfRangeU;

// ---
// Array change notification support
// ---
typedef enum {
  SNLF_OPERATION_ADD,
  SNLF_OPERATION_REMOVE,
  SNLF_OPERATION_MOVE,
  SNLF_OPERATION_REPLACE,
} SnlfArrayOperationType;

typedef struct {
  SnlfArrayOperationType operation;
  uint32_t oldIndex;
  uint32_t newIndex;
  uint32_t length;
} SnlfArrayChangedArgs;

typedef void (*SnlfArrayChangedHandler)(intptr_t, SnlfArrayChangedArgs, intptr_t);

// ---
// Core
// ---
typedef struct {
  const char *appPreferenceName;
} SnlfCoreStartupArgs;

SNLF_EXPORT SnlfCoreRef SnlfCoreInit(SnlfCoreStartupArgs args);
SNLF_EXPORT void SnlfCoreUninit(SnlfCoreRef core);

// ---
// Module
// ---
SNLF_EXPORT bool SnlfModuleLoadAll(SnlfCoreRef core);

// ---
// Graphics
// ---
typedef struct {
  timestamp_t timestamp;
  matrix4x4_t world;
} SnlfGraphicsUpdateParams;
typedef struct {
  const SnlfGraphicsContext *context;
  const CpsrTexture2D *renderTarget;
} SnlfGraphicsDrawParams;

SNLF_EXPORT const CpsrDevice *SnlfGraphicsContextGetDevice(const SnlfGraphicsContext *context);
SNLF_EXPORT const CpsrCommandBuffer *SnlfGraphicsContextGetCommandBuffer(const SnlfGraphicsContext *context);
SNLF_EXPORT CpsrGraphicsContext *SnlfGraphicsContextCreateGraphicsContext(const SnlfGraphicsContext *context);

typedef enum {
  SNLF_SHADER_VERTEX_DRAW,
  SNLF_SHADER_VERTEX_DRAW_SIMPLE,
  SNLF_SHADER_PIXEL_DRAW_HALF,
  SNLF_SHADER_PIXEL_DRAW_SINGLE,
  
  SNLF_SHADER_VERTEX_DRAW_COLOR,
  SNLF_SHADER_VERTEX_DRAW_COLOR_INSTANCED,
  SNLF_SHADER_PIXEL_DRAW_COLOR,
} SnlfShaderFunction;

SNLF_EXPORT const CpsrShaderFunction *SnlfGraphicsContextGetShaderFunction(const SnlfGraphicsContext *context, SnlfShaderFunction function);

SNLF_EXPORT void SnlfGraphicsContextDrawTexture(const SnlfGraphicsContext *context, const CpsrTexture2D *sourceTexture);

// ---
// Object
// ---
#define _SNLF_DEFINE_REFCOUNT_BASE(__NAME__) \
  inline int32_t Snlf##__NAME__##AddRef(Snlf##__NAME__##Ref obj) { \
    return SnlfObjectAddRef((SnlfObjectRef)obj); \
  } \
  \
  inline int32_t Snlf##__NAME__##GetRefCount(Snlf##__NAME__##Ref obj) { \
    return SnlfObjectGetRefCount((SnlfObjectRef)obj); \
  }

#define SNLF_DEFINE_REFCOUNT(__NAME__) \
  _SNLF_DEFINE_REFCOUNT_BASE(__NAME__) \
  \
  inline int32_t Snlf##__NAME__##Release(Snlf##__NAME__##Ref obj) { \
    return SnlfObjectRelease((SnlfObjectRef)obj); \
  }

#define SNLF_DEFINE_REFCOUNT_CUSTOM(__NAME__) \
  _SNLF_DEFINE_REFCOUNT_BASE(__NAME__) \
  \
  SNLF_EXPORT int32_t Snlf##__NAME__##Release(Snlf##__NAME__##Ref obj);

SNLF_EXPORT int32_t SnlfObjectAddRef(SnlfObjectRef obj);
SNLF_EXPORT int32_t SnlfObjectRelease(SnlfObjectRef obj);
SNLF_EXPORT int32_t SnlfObjectGetRefCount(SnlfObjectRef obj);

// ---
// Property
// ---
typedef enum {
  SNLF_PROPERTY_BACKGROUND_COLOR,
  SNLF_PROPERTY_BORDER_COLOR,
  SNLF_PROPERTY_FOREGROUND_COLOR,
  
  SNLF_PROPERTY_USER_DEFINE = 0x10000,
} SnlfPropertyIdentifier;

// ---
// Generator
// ---
struct _SnlfGraphicsGenerator {
  char generatorName[32];
  char friendlyName[32];
  
  intptr_t (*init)(const SnlfGraphicsContext *);
  void (*uninit)(intptr_t);
  void (*update)(intptr_t, SnlfGraphicsUpdateParams);
  void (*draw)(intptr_t, SnlfGraphicsDrawParams);
  
  uint32_t (*getPropertyCount)();
  bool (*getPropertyKeys)(identifier_t[], uint32_t);
};

SNLF_EXPORT bool SnlfGraphicsGeneratorRegister(SnlfCoreRef core, SnlfGraphicsGeneratorRef generator);
SNLF_EXPORT void SnlfGraphicsGeneratorUnregister(SnlfCoreRef core, SnlfGraphicsGeneratorRef generator);

typedef void (*SnlfGraphicsGeneratorProcedure)(SnlfGraphicsGeneratorRef generator, intptr_t param);
SNLF_EXPORT void SnlfEnumGraphicsGenerators(SnlfCoreRef core, SnlfGraphicsGeneratorProcedure enumFunc, intptr_t param);

// ---
// Transformer
// ---
struct _SnlfGraphicsTransformer {
  char transformerName[32];
  char friendlyName[32];
  bool sideEffect;
  
  intptr_t (*init)(const SnlfGraphicsContext *);
  void (*uninit)(intptr_t);
  void (*transform)(const CpsrTexture2D *, const CpsrTexture2D *);
};

// 1. YUV (444, 422, 420, 411)/RGB to linear RGB

// ---
// Input
// ---
typedef enum {
  SNLF_INPUT_UNKNOWN,
  
  SNLF_INPUT_CAMERA,
  SNLF_INPUT_WINDOW,
  SNLF_INPUT_DISPLAY,
  
  SNLF_INPUT_MICROPHONE,
  SNLF_INPUT_SPEAKER,
} SnlfInputType;

struct _SnlfInputDescriptor {
  char identifier[SNLF_INPUT_IDENTIFIER_LENGTH];
  char friendlyName[SNLF_INPUT_FRIENDLY_NAME_LENGTH];
  
  SnlfInputType type : 4;
  bool graphics      : 1; // has graphics
  bool sound         : 1; // has sound
  bool push          : 1; // use upload data
  bool unique        : 1; // cannot duplicate
  
  intptr_t (*init)(const SnlfInputDescriptor *);
  void (*uninit)(intptr_t);
  void (*activating)(intptr_t, const CpsrDevice *);
  void (*deactivated)(intptr_t);
};
SNLF_DEFINE_REFCOUNT(Input)

SNLF_EXPORT SnlfInputRef SnlfInputRegister(SnlfCoreRef core, SnlfInputDescriptor *descriptor);
SNLF_EXPORT identifier_t SnlfInputGetIdentifier(SnlfInputRef input);
SNLF_EXPORT const char *SnlfInputGetFriendlyName(SnlfInputRef input);

typedef void (*SnlfInputProcedure)(SnlfInputRef input, intptr_t param);
SNLF_EXPORT void SnlfEnumInputs(SnlfCoreRef core, SnlfInputProcedure enumFunc, intptr_t param);

// ---
// Output
// ---
struct _SnlfOutputDescriptor {
  char friendlyName[SNLF_OUTPUT_FRIENDLY_NAME_LENGTH];
  
  intptr_t (*init)(const SnlfOutputDescriptor *, const CpsrDevice *);
  void (*uninit)(intptr_t);
  void (*graphicsCallback)(intptr_t);
  void (*soundCallback)(intptr_t);
};

// ---
// Source
// ---
enum _SnlfBoundsType {
  SNLF_BOUNDS_NONE,          // Dot by Dot
  SNLF_BOUNDS_SCALE_TO_FILL, // Fit
  SNLF_BOUNDS_ASPECT_FIT,    // Underscan
  SNLF_BOUNDS_ASPECT_FILL,   // Overscan
  SNLF_BOUNDS_ASPECT_WIDTH,  // Fit to width
  SNLF_BOUNDS_ASPECT_HEIGHT, // Fit to height
};
typedef enum _SnlfBoundsType SnlfBoundsType;

SNLF_DEFINE_REFCOUNT_CUSTOM(Source)

SNLF_EXPORT SnlfSourceRef SnlfSourceCreate(SnlfCoreRef core);
SNLF_EXPORT SnlfSourceRef SnlfSourceCreateFromInput(SnlfInputRef input);
SNLF_EXPORT SnlfSourceRef SnlfSourceCreateFromGraphicsGenerator(SnlfCoreRef core, SnlfGraphicsGeneratorRef generator);

SNLF_EXPORT matrix4x4_t SnlfSourceGetTransform(SnlfSourceRef source);
SNLF_EXPORT void SnlfSourceSetTransform(SnlfSourceRef source, matrix4x4_t transform);

SNLF_EXPORT uint32_t SnlfSourceGetPropertyCount(SnlfSourceRef source);
SNLF_EXPORT bool SnlfSourceGetPropertyKeys(SnlfSourceRef source, identifier_t identifiers[], uint32_t count);
SNLF_EXPORT SnlfBox SnlfSourceGetProperty(SnlfSourceRef source, identifier_t identifier);
SNLF_EXPORT bool SnlfSourceSetProperty(SnlfSourceRef source, identifier_t identifier, SnlfBox value);

typedef void (*SnlfSourceProcedure)(SnlfSourceRef source, intptr_t param);
SNLF_EXPORT void SnlfSourceEnumChildSources(SnlfSourceRef source, SnlfSourceProcedure enumFunc, intptr_t param);

SNLF_EXPORT bool SnlfSourceAppend(SnlfSourceRef child, SnlfSourceRef parent);
SNLF_EXPORT bool SnlfSourceInsertAt(uint32_t index, SnlfSourceRef child, SnlfSourceRef parent);
SNLF_EXPORT bool SnlfSourceInsertBelow(SnlfSourceRef source, SnlfSourceRef sibling);
SNLF_EXPORT bool SnlfSourceInsertAbove(SnlfSourceRef source, SnlfSourceRef sibling);
SNLF_EXPORT bool SnlfSourceRemoveFromParent(SnlfSourceRef source);

SNLF_EXPORT intptr_t SnlfCoreRootSourceAddNotificationHandler(SnlfCoreRef core, SnlfArrayChangedHandler handler, intptr_t param);
SNLF_EXPORT bool SnlfCoreRootSourceRemoveNotificationHandler(SnlfCoreRef core, intptr_t handle);

SNLF_EXPORT intptr_t SnlfCoreSourceAddNotificationHandler(SnlfSourceRef source, SnlfArrayChangedHandler handler, intptr_t param);

// ---
// RootSource
// ---
SNLF_EXPORT SnlfSourceRef SnlfRootSourceGetCurrent(SnlfCoreRef core);
SNLF_EXPORT SnlfSourceRef SnlfRootSourceGetAt(SnlfCoreRef core, uint32_t index);

SNLF_EXPORT bool SnlfRootSourceAppend(SnlfSourceRef source);

// ---
// Transition
// ---
SNLF_EXPORT bool SnlfTransitionDispatch(SnlfCoreRef core, SnlfSourceRef source);

typedef struct {
  SnlfSourceRef previousSource;
  SnlfSourceRef currentSource;
} SnlfSourceChangedArgs;

typedef void (*SnlfSourceChangedHandler)(SnlfCoreRef, SnlfSourceChangedArgs, intptr_t);

SNLF_EXPORT intptr_t SnlfTransitionAddNotificationHandler(SnlfCoreRef core, SnlfSourceChangedHandler handler, intptr_t param);
SNLF_EXPORT bool SnlfTransitionRemoveNotificationHandler(SnlfCoreRef core, intptr_t handle);

// Cleans up
#undef SNLF_DEFINE_REFCOUNT

#ifdef __cplusplus
}
#endif

#endif // _SNLF_CORE_H
