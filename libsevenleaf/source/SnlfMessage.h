#ifndef _SNLF_MESSAGE_H
#define _SNLF_MESSAGE_H

#include "SnlfCore+Private.h"

#define SNLF_MESSAGE_DEFINE_BEGIN(__NAME__) \
  typedef struct _Snlf##__NAME__##Message Snlf##__NAME__##Message; \
  struct _Snlf##__NAME__##Message {\
    SnlfMessageType type; \
    identifier_t uniqueIdentifier; \
    timestamp_t timestamp; \
    intptr_t sender;
#define SNLF_MESSAGE_DEFINE_END };

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  // Target: All
  SNLF_MESSAGE_DEBUG,
  
  // Target: Graphics & Sounds root
  SNLF_MESSAGE_TERMINATE,
  SNLF_MESSAGE_REGISTER_MONITOR,
  SNLF_MESSAGE_UNREGISTER_MONITOR,
  
  // Transition
  SNLF_MESSAGE_TRANSITION_CHANGE_SCENE,
  
  // Source
  SNLF_MESSAGE_SOURCE_ADD,
  SNLF_MESSAGE_SOURCE_REMOVE,
  SNLF_MESSAGE_SOURCE_ANIMATION,
} SnlfMessageType;

typedef enum {
  SNLF_MESSAGE_TARGET_ALL = 0xF,
  SNLF_MESSAGE_TARGET_GRAPHICS = 1 << 0,
  SNLF_MESSAGE_TARGET_SOUND = 1 << 1,
} SnlfMessageTarget;

// Basic
SNLF_MESSAGE_DEFINE_BEGIN(Basic)
SNLF_MESSAGE_DEFINE_END

// Animation
SNLF_MESSAGE_DEFINE_BEGIN(Animation)
  duration_t duration;
  //CpsrSingle4x4 (*positionMatrixFunction)(normalized_time_t time);
  //CpsrSingle4x4 (*colorMatrixFunction)(normalized_time_t time);
  // sound matrix
SNLF_MESSAGE_DEFINE_END

SnlfBasicMessage *SnlfMessageCreateFromSource(SnlfMessageType type, SnlfSourceRef source);
SnlfBasicMessage *SnlfMessageGet();

void SnlfMessageDispatchToRoot(SnlfCoreRef core, const SnlfBasicMessage *message);
void SnlfMessageDispatchToSource(SnlfSourceRef source, const SnlfBasicMessage *message);

#ifdef __cplusplus
}
#endif

#endif // _SNLF_CORE_PRIVATE_H
