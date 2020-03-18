#ifndef _SIMD_MATRIX4X4_H
#define _SIMD_MATRIX4X4_H

#include "float32x4_t.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  float32x4_t v1, v2, v3, v4;
} matrix4x4_t;

static inline matrix4x4_t _SIMD_CALLCONV matrix4x4_idt() {
  matrix4x4_t m;
  m.v1 = float32x4_initv(1.F, 0.F, 0.F, 0.F);
  m.v2 = float32x4_initv(0.F, 1.F, 0.F, 0.F);
  m.v3 = float32x4_initv(0.F, 0.F, 1.F, 0.F);
  m.v4 = float32x4_initv(0.F, 0.F, 0.F, 1.F);
  return m;
}

// ---
// Translate
// ---
static inline matrix4x4_t _SIMD_CALLCONV matrix4x4_tx(float32_t offset) {
  matrix4x4_t m;
  // clang-format off
  m.v1 = float32x4_initv(1.F,    0.F, 0.F, 0.F);
  m.v2 = float32x4_initv(0.F,    1.F, 0.F, 0.F);
  m.v3 = float32x4_initv(0.F,    0.F, 1.F, 0.F);
  m.v4 = float32x4_initv(offset, 0.F, 0.F, 1.F);
  // clang-format on
  return m;
}

static inline matrix4x4_t _SIMD_CALLCONV matrix4x4_ty(float32_t offset) {
  matrix4x4_t m;
  // clang-format off
  m.v1 = float32x4_initv(1.F, 0.F,    0.F, 0.F);
  m.v2 = float32x4_initv(0.F, 1.F,    0.F, 0.F);
  m.v3 = float32x4_initv(0.F, 0.F,    1.F, 0.F);
  m.v4 = float32x4_initv(0.F, offset, 0.F, 1.F);
  // clang-format on
  return m;
}

static inline matrix4x4_t _SIMD_CALLCONV matrix4x4_tz(float32_t offset) {
  matrix4x4_t m;
  // clang-format off
  m.v1 = float32x4_initv(1.F, 0.F, 0.F,    0.F);
  m.v2 = float32x4_initv(0.F, 1.F, 0.F,    0.F);
  m.v3 = float32x4_initv(0.F, 0.F, 1.F,    0.F);
  m.v4 = float32x4_initv(0.F, 0.F, offset, 1.F);
  // clang-format on
  return m;
}

static inline matrix4x4_t _SIMD_CALLCONV float32m3x2_trans(float32_t offsetX, float32_t offsetY, float32_t offsetZ) {
  matrix4x4_t m;
  // clang-format off
  m.v1 = float32x4_initv(1.F,     0.F,     0.F,     0.F);
  m.v2 = float32x4_initv(0.F,     1.F,     0.F,     0.F);
  m.v3 = float32x4_initv(0.F,     0.F,     1.F,     0.F);
  m.v4 = float32x4_initv(offsetX, offsetY, offsetZ, 1.F);
  // clang-format on
  return m;
}

// ---
// Scale
// ---
static inline matrix4x4_t _SIMD_CALLCONV matrix4x4_sx(float32_t scale) {
  matrix4x4_t m;
  // clang-format off
  m.v1 = float32x4_initv(scale, 0.F, 0.F, 0.F);
  m.v2 = float32x4_initv(0.F,   1.F, 0.F, 0.F);
  m.v3 = float32x4_initv(0.F,   0.F, 1.F, 0.F);
  m.v4 = float32x4_initv(0.F,   0.F, 0.F, 1.F);
  // clang-format on
  return m;
}

static inline matrix4x4_t _SIMD_CALLCONV matrix4x4_sy(float32_t scale) {
  matrix4x4_t m;
  // clang-format off
  m.v1 = float32x4_initv(1.F, 0.F,   0.F, 0.F);
  m.v2 = float32x4_initv(0.F, scale, 0.F, 0.F);
  m.v3 = float32x4_initv(0.F, 0.F,   1.F, 0.F);
  m.v4 = float32x4_initv(0.F, 0.F,   0.F, 1.F);
  // clang-format on
  return m;
}

static inline matrix4x4_t _SIMD_CALLCONV matrix4x4_sz(float32_t scale) {
  matrix4x4_t m;
  // clang-format off
  m.v1 = float32x4_initv(1.F, 0.F, 0.F,   0.F);
  m.v2 = float32x4_initv(0.F, 1.F, 0.F,   0.F);
  m.v3 = float32x4_initv(0.F, 0.F, scale, 0.F);
  m.v4 = float32x4_initv(0.F, 0.F, 0.F,   1.F);
  // clang-format on
  return m;
}

static inline matrix4x4_t _SIMD_CALLCONV matrix4x4_scale(float32_t scaleX, float32_t scaleY, float32_t scaleZ) {
  matrix4x4_t m;
  // clang-format off
  m.v1 = float32x4_initv(scaleX, 0.F,    0.F,    0.F);
  m.v2 = float32x4_initv(0.F,    scaleY, 0.F,    0.F);
  m.v3 = float32x4_initv(0.F,    0.F,    scaleZ, 0.F);
  m.v4 = float32x4_initv(0.F,    0.F,    0.F,    1.F);
  // clang-format on
  return m;
}

// ---
// Skew
// ---
static inline matrix4x4_t _SIMD_CALLCONV matrix4x4_skewxy(float32_t radian) {
  float32_t t = tanf(radian);
  
  matrix4x4_t m;
  // clang-format off
  m.v1 = float32x4_initv(1.F, 0.F, 0.F, 0.F);
  m.v2 = float32x4_initv(t,   1.F, 0.F, 0.F);
  m.v3 = float32x4_initv(0.F, 0.F, 1.F, 0.F);
  m.v4 = float32x4_initv(0.F, 0.F, 0.F, 1.F);
  // clang-format on
  return m;
}

static inline matrix4x4_t _SIMD_CALLCONV matrix4x4_skewxz(float32_t radian) {
  float32_t t = tanf(radian);
  
  matrix4x4_t m;
  // clang-format off
  m.v1 = float32x4_initv(1.F, 0.F, 0.F, 0.F);
  m.v2 = float32x4_initv(0.F, 1.F, 0.F, 0.F);
  m.v3 = float32x4_initv(t,   0.F, 1.F, 0.F);
  m.v4 = float32x4_initv(0.F, 0.F, 0.F, 1.F);
  // clang-format on
  return m;
}

static inline matrix4x4_t _SIMD_CALLCONV matrix4x4_skewyx(float32_t radian) {
  float32_t t = tanf(radian);
  
  matrix4x4_t m;
  // clang-format off
  m.v1 = float32x4_initv(1.F, t,   0.F, 0.F);
  m.v2 = float32x4_initv(0.F, 1.F, 0.F, 0.F);
  m.v3 = float32x4_initv(0.F, 0.F, 1.F, 0.F);
  m.v4 = float32x4_initv(0.F, 0.F, 0.F, 1.F);
  // clang-format on
  return m;
}

static inline matrix4x4_t _SIMD_CALLCONV matrix4x4_skewyz(float32_t radian) {
  float32_t t = tanf(radian);
  
  matrix4x4_t m;
  // clang-format off
  m.v1 = float32x4_initv(1.F, 0.F, 0.F, 0.F);
  m.v2 = float32x4_initv(0.F, 1.F, 0.F, 0.F);
  m.v3 = float32x4_initv(0.F, t,   1.F, 0.F);
  m.v4 = float32x4_initv(0.F, 0.F, 0.F, 1.F);
  // clang-format on
  return m;
}

static inline matrix4x4_t _SIMD_CALLCONV matrix4x4_skewzx(float32_t radian) {
  float32_t t = tanf(radian);
  
  matrix4x4_t m;
  // clang-format off
  m.v1 = float32x4_initv(1.F, 0.F, t,   0.F);
  m.v2 = float32x4_initv(0.F, 1.F, 0.F, 0.F);
  m.v3 = float32x4_initv(0.F, 0.F, 1.F, 0.F);
  m.v4 = float32x4_initv(0.F, 0.F, 0.F, 1.F);
  // clang-format on
  return m;
}

static inline matrix4x4_t _SIMD_CALLCONV matrix4x4_skewzy(float32_t radian) {
  float32_t t = tanf(radian);
  
  matrix4x4_t m;
  // clang-format off
  m.v1 = float32x4_initv(1.F, 0.F, 0.F, 0.F);
  m.v2 = float32x4_initv(0.F, 1.F, t,   0.F);
  m.v3 = float32x4_initv(0.F, 0.F, 1.F, 0.F);
  m.v4 = float32x4_initv(0.F, 0.F, 0.F, 1.F);
  // clang-format on
  return m;
}

// ---
// Rotation
// ---
static inline matrix4x4_t _SIMD_CALLCONV matrix4x4_rx(float32_t radian) {
  float32_t c = cosf(radian);
  float32_t s = sinf(radian);
  
  matrix4x4_t m;
  // clang-format off
  m.v1 = float32x4_initv(1.F, 0.F, 0.F, 0.F);
  m.v2 = float32x4_initv(0.F, c,   s,   0.F);
  m.v3 = float32x4_initv(0.F, -s,  c,   0.F);
  m.v4 = float32x4_initv(0.F, 0.F, 0.F, 1.F);
  // clang-format on
  return m;
}

static inline matrix4x4_t _SIMD_CALLCONV matrix4x4_ry(float32_t radian) {
  float32_t c = cosf(radian);
  float32_t s = sinf(radian);
  
  matrix4x4_t m;
  // clang-format off
  m.v1 = float32x4_initv(c,   0.F, -s,  0.F);
  m.v2 = float32x4_initv(0.F, 1.F, 0.F, 0.F);
  m.v3 = float32x4_initv(s,   0.F, c,   0.F);
  m.v4 = float32x4_initv(0.F, 0.F, 0.F, 1.F);
  // clang-format on
  return m;
}

static inline matrix4x4_t _SIMD_CALLCONV matrix4x4_rz(float32_t radian) {
  float32_t c = cosf(radian);
  float32_t s = sinf(radian);
  
  matrix4x4_t m;
  // clang-format off
  m.v1 = float32x4_initv(c,   s,   0.F, 0.F);
  m.v2 = float32x4_initv(-s,  c,   0.F, 0.F);
  m.v3 = float32x4_initv(s,   0.F, 1.F, 0.F);
  m.v4 = float32x4_initv(0.F, 0.F, 0.F, 1.F);
  // clang-format on
  return m;
}

// ---
// Orthographic
// ---
static inline matrix4x4_t _SIMD_CALLCONV matrix4x4_ortho(float32_t width, float32_t height, float32_t near, float32_t far) {
  float32_t clip = far - near;
  
  matrix4x4_t m;
  // clang-format off
  m.v1 = float32x4_initv(2.F / width, 0.F,          0.F,         0.F);
  m.v2 = float32x4_initv(0.F,         2.F / height, 0.F,         0.F);
  m.v3 = float32x4_initv(0.F,         0.F,          1.F / clip,  0.F);
  m.v4 = float32x4_initv(0.F,         0.F,          near / clip, 1.F);
  // clang-format on
  return m;
}

static inline matrix4x4_t _SIMD_CALLCONV matrix4x4_ortho_offcenter(float32_t left, float32_t right, float32_t bottom, float32_t top, float32_t near, float32_t far) {
  float32_t width = right - left;
  float32_t height = top - bottom;
  float32_t clip = far - near;
  
  matrix4x4_t m;
  // clang-format off
  m.v1 = float32x4_initv(2.F / width,             0.F,                      0.F,         0.F);
  m.v2 = float32x4_initv(0.F,                     2.F / height,             0.F,         0.F);
  m.v3 = float32x4_initv(0.F,                     0.F,                      1.F / clip,  0.F);
  m.v4 = float32x4_initv(-(right + left) / width, -(top + bottom) / height, near / clip, 1.F);
  // clang-format on
  return m;
}

// ---
// Transform
// ---
static inline float32x4_t _SIMD_CALLCONV matrix4x4_transform(float32x4_t a, matrix4x4_t m) {
  float32x4_t ret;
#if defined(_SIMD_ARM_NEON)
  float32x2_t lo = vget_low_f32(a);
  float32x2_t vx = vmulq_lane_f32(m.v1, lo, 0);
  float32x2_t vy = vmulq_lane_f32(m.v2, lo, 1);
  float32x2_t hi = vget_low_f32(a);
#ifdef _SIMD_ARM64
  float32x2_t vz = vfmaq_lane_f32(vx, m.v3, hi, 0);
  float32x2_t vw = vfmaq_lane_f32(vy, m.v4, hi, 1);
#else
  float32x2_t vz = vmlaq_lane_f32(vx, m.v3, hi, 0);
  float32x2_t vw = vmlaq_lane_f32(vy, m.v4, hi, 1);
#endif
  ret = vaddq_f32(vz, vw);
#elif defined(_SIMD_X86_SSE)
  __m128 a0 = _simd_mm_shuffle_ps(a, _MM_SHUFFLE(0, 0, 0, 0));
  __m128 a1 = _simd_mm_shuffle_ps(a, _MM_SHUFFLE(1, 1, 1, 1));
  __m128 a2 = _simd_mm_shuffle_ps(a, _MM_SHUFFLE(2, 2, 2, 2));
  __m128 a3 = _simd_mm_shuffle_ps(a, _MM_SHUFFLE(3, 3, 3, 3));
  
  __m128 vx = _mm_mul_ps(a0, m.v1);
  __m128 vy = _mm_mul_ps(a1, m.v2);
#ifdef _SIMD_X86_FMA3
  __m128 vz = _mm_fmadd_ps(a3, m.v3, vx);
  __m128 vw = _mm_fmadd_ps(a4, m.v4, vy);
#else
  __m128 vz = _mm_add_ps(_mm_mul_ps(a2, m.v3), vx);
  __m128 vw = _mm_add_ps(_mm_mul_ps(a3, m.v4), vy);
#endif
  ret = _mm_add_ps(vz, vw);
#else
  ret.f32[0] = m.v1.f32[0] * a.f32[0] + m.v2.f32[0] * a.f32[1] + m.v3.f32[0] * a.f32[2] + m.v4.f32[0] * a.f32[3];
  ret.f32[1] = m.v1.f32[1] * a.f32[0] + m.v2.f32[1] * a.f32[1] + m.v3.f32[1] * a.f32[2] + m.v4.f32[1] * a.f32[3];
  ret.f32[2] = m.v1.f32[2] * a.f32[0] + m.v2.f32[2] * a.f32[1] + m.v3.f32[2] * a.f32[2] + m.v4.f32[2] * a.f32[3];
  ret.f32[3] = m.v1.f32[3] * a.f32[0] + m.v2.f32[3] * a.f32[1] + m.v3.f32[3] * a.f32[2] + m.v4.f32[3] * a.f32[3];
#endif
  return ret;
}

// ---
// Arithmetic
// ---
static inline matrix4x4_t _SIMD_CALLCONV matrix4x4_add(matrix4x4_t a, matrix4x4_t b) {
  matrix4x4_t m;
  m.v1 = float32x4_add(a.v1, b.v1);
  m.v2 = float32x4_add(a.v2, b.v2);
  m.v3 = float32x4_add(a.v3, b.v3);
  m.v4 = float32x4_add(a.v4, b.v4);
  return m;
}

static inline matrix4x4_t _SIMD_CALLCONV matrix4x4_mul(matrix4x4_t a, matrix4x4_t b) {
  matrix4x4_t m;
  m.v1 = matrix4x4_transform(a.v1, b);
  m.v2 = matrix4x4_transform(a.v2, b);
  m.v3 = matrix4x4_transform(a.v3, b);
  m.v4 = matrix4x4_transform(a.v4, b);
  return m;
}

#ifdef __cplusplus
}
#endif

#endif  // _SIMD_MATRIX4X4_H
