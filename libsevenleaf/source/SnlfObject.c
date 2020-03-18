#include "SnlfCore+Private.h"

#include <osutil_atomic.h>

int32_t SnlfObjectAddRef(SnlfObjectRef obj) {
  int32_t ret = osutil_atomic_fetch_increment32(&obj->refCount);
#ifdef DEBUG_REFERENCE_COUNT
  SnlfVerboseLogFormat("Add reference count: %d on %p (SnlfObject)", ret + 1, obj);
#endif
  return ++ret;
}

int32_t SnlfObjectRelease(SnlfObjectRef obj) {
  int32_t ret = osutil_atomic_fetch_decrement32(&obj->refCount);
#ifdef DEBUG_REFERENCE_COUNT
  SnlfVerboseLogFormat("Release reference count: %d on %p (SnlfObject)", ret - 1, obj);
#endif
  if (ret == 1) {
    SnlfDealloc(obj);
  }
  return --ret;
}

int32_t SnlfObjectGetRefCount(SnlfObjectRef obj) {
  int32_t ret = osutil_atomic_load32(&obj->refCount);
  return ret;
}
