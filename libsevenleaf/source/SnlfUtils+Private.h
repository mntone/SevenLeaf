#ifndef _SNLF_UTILS_H
#define _SNLF_UTILS_H

#include "SnlfConfig.h"

#include <pthread.h>

#ifdef __cplusplus
extern "C" {
#endif

// ---
// Mutex
// ---
static inline bool SnlfMutexCreate(pthread_mutex_t *mutex) {
  pthread_mutexattr_t mutexAttr;
  if (pthread_mutexattr_init(&mutexAttr)
#ifndef NDEBUG
      || pthread_mutexattr_settype(&mutexAttr, PTHREAD_MUTEX_ERRORCHECK)
#else
      || pthread_mutexattr_settype(&mutexAttr, PTHREAD_MUTEX_NORMAL)
#endif
      || pthread_mutex_init(mutex, &mutexAttr)) {
    SnlfMutexCreationError();
    return true;
  }
  
  if (pthread_mutexattr_destroy(&mutexAttr)) {
    SnlfMutexCreationDisposingError();
  }
  return false;
}

static inline bool SnlfRecursiveMutexCreate(pthread_mutex_t *mutex) {
  pthread_mutexattr_t mutexAttr;
  if (pthread_mutexattr_init(&mutexAttr)
      || pthread_mutexattr_settype(&mutexAttr, PTHREAD_MUTEX_RECURSIVE)
      || pthread_mutex_init(mutex, &mutexAttr)) {
    SnlfMutexCreationError();
    return true;
  }
  
  if (pthread_mutexattr_destroy(&mutexAttr)) {
    SnlfMutexCreationDisposingError();
  }
  return false;
}

static inline bool SnlfMutexDestroy(pthread_mutex_t *mutex) {
  if (pthread_mutex_destroy(mutex)) {
    SnlfMutexDestructionError();
    return true;
  }
  return false;
}

#ifdef __cplusplus
}
#endif

#endif // _SNLF_UTILS_H
