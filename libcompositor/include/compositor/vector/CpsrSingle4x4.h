#ifndef _CPSR_SINGLE4X4_H
#define _CPSR_SINGLE4X4_H

#include <assert.h>
#include "float32x4_t.h"

#ifdef __cplusplus
extern "C" {
#endif

struct _CpsrSingle4x4 {
  union {
    struct {
      float m11, m12, m13, m14;
      float m21, m22, m23, m24;
      float m31, m32, m33, m34;
      float m41, m42, m43, m44;
    };
    struct {
      float32x4_t v1, v2, v3, v4;
    };
  };
};
typedef struct _CpsrSingle4x4 CpsrSingle4x4;

static inline void _SIMD_CALLCONV
CpsrSingle4x4Perspective(CpsrSingle4x4 *m, float width, float height, float near, float far) {
  assert(near >= 0.F);
  assert(far > near);

  float near2 = 2.F * near;
  float clip = far - near;
  // clang-format off
  m->m11 = near2 / width; m->m12 = 0.F;            m->m13 = 0.F;               m->m14 = 0.F;
  m->m21 = 0.F;           m->m22 = near2 / height; m->m23 = 0.F;               m->m24 = 0.F;
  m->m31 = 0.F;           m->m32 = 0.F;            m->m33 = far / clip;        m->m34 = 1.F;
  m->m41 = 0.F;           m->m42 = 0.F;            m->m43 = near * far / clip; m->m44 = 0.F;
  // clang-format on
}

static inline void _SIMD_CALLCONV CpsrSingle4x4PerspectiveOffCenter(CpsrSingle4x4 *m,
                                                                    float left,
                                                                    float right,
                                                                    float bottom,
                                                                    float top,
                                                                    float near,
                                                                    float far) {
  assert(near > 0.F);
  assert(far > near);

  float near2 = 2.F * near;
  float width = right - left;
  float height = top - bottom;
  float clip = far - near;
  // clang-format off
  m->m11 = near2 / width;          m->m12 = 0.F;                     m->m13 = 0.F;               m->m14 = 0.F;
  m->m21 = 0.F;                    m->m22 = near2 / height;          m->m23 = 0.F;               m->m24 = 0.F;
  m->m31 = (right + left) / width; m->m32 = (top + bottom) / height; m->m33 = far / clip;        m->m34 = 1.F;
  m->m41 = 0.F;                    m->m42 = 0.F;                     m->m43 = near * far / clip; m->m44 = 0.F;
  // clang-format on
}

static inline void CpsrSingle4x4Transpose(CpsrSingle4x4 *dst, const CpsrSingle4x4 *m) {
  float32x4x2_t a0 = float32x4_xxyy_zzww(m->v1, m->v3);
  float32x4x2_t a1 = float32x4_xxyy_zzww(m->v2, m->v4);
  float32x4x2_t b0 = float32x4_xxyy_zzww(a0.val[0], a1.val[0]);
  float32x4x2_t b1 = float32x4_xxyy_zzww(a0.val[1], a1.val[1]);
  dst->v1 = b0.val[0];
  dst->v2 = b0.val[1];
  dst->v3 = b1.val[0];
  dst->v4 = b1.val[1];
}

#ifdef __cplusplus
}
#endif

#endif  // _CPSR_SINGLE4X4_H
