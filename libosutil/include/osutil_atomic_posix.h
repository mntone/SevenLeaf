#ifndef _OSUTIL_ATOMIC_POSIX_H
#define _OSUTIL_ATOMIC_POSIX_H

#include "osutil_base.h"

#include <stdatomic.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef _Atomic(int32_t)  osutil_atomic_int32_t;
typedef _Atomic(int64_t)  osutil_atomic_int64_t;
typedef _Atomic(intptr_t) osutil_atomic_intptr_t;


static inline int32_t osutil_atomic_load32(const osutil_atomic_int32_t *obj) {
  return atomic_load(obj);
}
static inline int64_t osutil_atomic_load64(const osutil_atomic_int64_t *obj) {
  return atomic_load(obj);
}
static inline intptr_t osutil_atomic_load_pointer(const osutil_atomic_intptr_t *obj) {
  return atomic_load(obj);
}

static inline void osutil_atomic_store32(osutil_atomic_int32_t *obj, int32_t desired) {
  atomic_store(obj, desired);
}
static inline void osutil_atomic_store64(osutil_atomic_int64_t *obj, int64_t desired) {
  atomic_store(obj, desired);
}
static inline void osutil_atomic_store_pointer(osutil_atomic_intptr_t *obj, intptr_t desired) {
  atomic_store(obj, desired);
}

static inline int32_t osutil_atomic_fetch_add32(osutil_atomic_int32_t *obj, int32_t arg) {
  return atomic_fetch_add(obj, arg);
}
static inline int64_t osutil_atomic_fetch_add64(osutil_atomic_int64_t *obj, int64_t arg) {
  return atomic_fetch_add(obj, arg);
}
static inline intptr_t osutil_atomic_fetch_add_pointer(osutil_atomic_intptr_t *obj, intptr_t arg) {
  return atomic_fetch_add(obj, arg);
}

static inline int32_t osutil_atomic_fetch_increment32(osutil_atomic_int32_t *obj) {
  return atomic_fetch_add(obj, 1);
}
static inline int64_t osutil_atomic_fetch_increment64(osutil_atomic_int64_t *obj) {
  return atomic_fetch_add(obj, 1);
}
static inline intptr_t osutil_atomic_fetch_increment_pointer(osutil_atomic_intptr_t *obj) {
  return atomic_fetch_add(obj, 1);
}

static inline int32_t osutil_atomic_fetch_sub32(osutil_atomic_int32_t *obj, int32_t arg) {
  return atomic_fetch_sub(obj, arg);
}
static inline int64_t osutil_atomic_fetch_sub64(osutil_atomic_int64_t *obj, int64_t arg) {
  return atomic_fetch_sub(obj, arg);
}
static inline intptr_t osutil_atomic_fetch_sub_pointer(osutil_atomic_intptr_t *obj, intptr_t arg) {
  return atomic_fetch_sub(obj, arg);
}

static inline int32_t osutil_atomic_fetch_decrement32(osutil_atomic_int32_t *obj) {
  return atomic_fetch_sub(obj, 1);
}
static inline int64_t osutil_atomic_fetch_decrement64(osutil_atomic_int64_t *obj) {
  return atomic_fetch_sub(obj, 1);
}
static inline intptr_t osutil_atomic_fetch_decrement_pointer(osutil_atomic_intptr_t *obj) {
  return atomic_fetch_sub(obj, 1);
}

#ifdef __cplusplus
}
#endif

#endif  // _OSUTIL_ATOMIC_POSIX_H
