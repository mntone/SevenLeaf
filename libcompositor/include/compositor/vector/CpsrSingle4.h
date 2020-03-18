#ifndef _CPSR_SINGLE4_H
#define _CPSR_SINGLE4_H

#include "../CpsrTypedefs.h"
#include "float32x4_t.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef float32x4_t CpsrSingle4;

static inline void CpsrSingle4Ceil(CpsrSingle4 *dst, const CpsrSingle4 *v) {
#ifdef __SSE4_1__
  *dst = _mm_ceil_ps(*v);
#else
  dst->x = ceilf(v->x);
  dst->y = ceilf(v->y);
  dst->z = ceilf(v->z);
  dst->w = ceilf(v->w);
#endif
}

static inline void CpsrSingle4Floor(CpsrSingle4 *dst, CpsrSingle4 *v) {
#ifdef __SSE4_1__
  *dst = _mm_floor_ps(*v);
#else
  dst->x = floorf(v->x);
  dst->y = floorf(v->y);
  dst->z = floorf(v->z);
  dst->w = floorf(v->w);
#endif
}

static inline void CpsrSingle4Round(CpsrSingle4 *dst, CpsrSingle4 *v) {
#ifdef __SSE4_1__
  *dst = _mm_round_ps(*v, _MM_FROUND_RINT);
#else
  dst->x = roundf(v->x);
  dst->y = roundf(v->y);
  dst->z = roundf(v->z);
  dst->w = roundf(v->w);
#endif
}

static inline void CpsrSingle4Trunc(CpsrSingle4 *dst, CpsrSingle4 *v) {
#ifdef __SSE4_1__
  *dst = _mm_round_ps(*v, _MM_FROUND_TRUNC);
#else
  dst->x = truncf(v->x);
  dst->y = truncf(v->y);
  dst->z = truncf(v->z);
  dst->w = truncf(v->w);
#endif
}

#ifdef __cplusplus
}
#endif

#endif // _CPSR_SINGLE4_H
