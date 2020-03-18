#ifndef _SNLF_CONTAINERS_LOCKFREEQUEUE_PROTOTYPES_H
#define _SNLF_CONTAINERS_LOCKFREEQUEUE_PROTOTYPES_H

#ifdef _MSC_VER
#define _Atomic(__TYPE__) volatile __TYPE__

typedef _Atomic(intptr_t) atomic_intptr_t;

static inline bool atomic_compare_exchange_strong(atomic_intptr_t* object, intptr_t* expected, intptr_t desired) {
  return InterlockedCompareExchangePointer(object, *expected, desired) == *object;
}
static inline bool atomic_compare_exchange_weak(atomic_intptr_t* object, intptr_t* expected, intptr_t desired) {
  return atomic_compare_exchange_strong(object, expected, desired);
} 

#else
#include <stdatomic.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

struct _SnlfLockFreeQueue {
  atomic_intptr_t head;
  atomic_intptr_t tail;
};
typedef struct _SnlfLockFreeQueue SnlfLockFreeQueue;

#ifdef __cplusplus
}
#endif

#endif // _SNLF_CONTAINERS_LOCKFREEQUEUE_PROTOTYPES_H
