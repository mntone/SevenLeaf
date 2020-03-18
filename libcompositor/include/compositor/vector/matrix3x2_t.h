#ifndef _SIMD_MATRIX3X2_H
#define _SIMD_MATRIX3X2_H

#include "simd_base.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  float m11, m12;
  float m21, m22;
  float m31, m32;
} matrix3x2_t;

static inline matrix3x2_t _SIMD_CALLCONV matrix3x2_idt() {
  matrix3x2_t m;
  // clang-format off
  m.m11 = 1.F; m.m12 = 0.F;
  m.m21 = 0.F; m.m22 = 1.F;
  m.m31 = 0.F; m.m32 = 0.F;
  // clang-format on
  return m;
}

// ---
// Translate
// ---
static inline matrix3x2_t _SIMD_CALLCONV matrix3x2_tx(float32_t offset) {
  matrix3x2_t m;
  // clang-format off
  m.m11 = 1.F;    m.m12 = 0.F;
  m.m21 = 0.F;    m.m22 = 1.F;
  m.m31 = offset; m.m32 = 0.F;
  // clang-format on
  return m;
}

static inline matrix3x2_t _SIMD_CALLCONV matrix3x2_ty(float32_t offset) {
  matrix3x2_t m;
  // clang-format off
  m.m11 = 1.F; m.m12 = 0.F;
  m.m21 = 0.F; m.m22 = 1.F;
  m.m31 = 0.F; m.m32 = offset;
  // clang-format on
  return m;
}

static inline matrix3x2_t _SIMD_CALLCONV matrix3x2_trans(float32_t offsetX, float32_t offsetY) {
  matrix3x2_t m;
  // clang-format off
  m.m11 = 1.F;     m.m12 = 0.F;
  m.m21 = 0.F;     m.m22 = 1.F;
  m.m31 = offsetX; m.m32 = offsetY;
  // clang-format on
  return m;
}

// ---
// Scale
// ---
static inline matrix3x2_t _SIMD_CALLCONV matrix3x2_sx(float32_t scale) {
  matrix3x2_t m;
  // clang-format off
  m.m11 = scale; m.m12 = 0.F;
  m.m21 = 0.F;   m.m22 = 1.F;
  m.m31 = 0.F;   m.m32 = 0.F;
  // clang-format on
  return m;
}

static inline matrix3x2_t _SIMD_CALLCONV matrix3x2_sx_offcenter(float32_t scale, float32x2_t center) {
  float32_t tx = center.x * (1.F - scale);

  matrix3x2_t m;
  // clang-format off
  m.m11 = scale; m.m12 = 0.F;
  m.m21 = 0.F;   m.m22 = 1.F;
  m.m31 = tx;    m.m32 = 0.F;
  // clang-format on
  return m;
}

static inline matrix3x2_t _SIMD_CALLCONV matrix3x2_sy(float32_t scale) {
  matrix3x2_t m;
  // clang-format off
  m.m11 = 1.F; m.m12 = 0.F;
  m.m21 = 0.F; m.m22 = scale;
  m.m31 = 0.F; m.m32 = 0.F;
  // clang-format on
  return m;
}

static inline matrix3x2_t _SIMD_CALLCONV matrix3x2_sy_offcenter(float32_t scale, float32x2_t center) {
  float32_t ty = center.y * (1.F - scale);

  matrix3x2_t m;
  // clang-format off
  m.m11 = 1.F; m.m12 = 0.F;
  m.m21 = 0.F; m.m22 = scale;
  m.m31 = 0.F; m.m32 = ty;
  // clang-format on
  return m;
}

static inline matrix3x2_t _SIMD_CALLCONV matrix3x2_scale(float32_t scaleX, float32_t scaleY) {
  matrix3x2_t m;
  // clang-format off
  m.m11 = scaleX; m.m12 = 0.F;
  m.m21 = 0.F;    m.m22 = scaleY;
  m.m31 = 0.F;    m.m32 = 0.F;
  // clang-format on
  return m;
}

static inline matrix3x2_t _SIMD_CALLCONV matrix3x2_scale_offcenter(float32_t scaleX, float32_t scaleY, float32x2_t center) {
  float32_t tx = center.x * (1.F - scaleX);
  float32_t ty = center.y * (1.F - scaleY);

  matrix3x2_t m;
  // clang-format off
  m.m11 = scaleX; m.m12 = 0.F;
  m.m21 = 0.F;    m.m22 = scaleY;
  m.m31 = tx;     m.m32 = ty;
  // clang-format on
  return m;
}

// ---
// Skew
// ---
static inline matrix3x2_t _SIMD_CALLCONV matrix3x2_skewx(float32_t radian) {
  float32_t sk = tanf(radian);

  matrix3x2_t m;
  // clang-format off
  m.m11 = 1.F; m.m12 = 0.F;
  m.m21 = sk;  m.m22 = 1.F;
  m.m31 = 0.F; m.m32 = 0.F;
  // clang-format on
  return m;
}

static inline matrix3x2_t _SIMD_CALLCONV matrix3x2_skewx_offcenter(float32_t radian, float32x2_t center) {
  float32_t sk = tanf(radian);
  float32_t t = -center.x * sk;

  matrix3x2_t m;
  // clang-format off
  m.m11 = 1.F; m.m12 = 0.F;
  m.m21 = sk;  m.m22 = 1.F;
  m.m31 = t;   m.m32 = 0.F;
  // clang-format on
  return m;
}

static inline matrix3x2_t _SIMD_CALLCONV matrix3x2_skewy(float32_t radian) {
  float32_t sk = tanf(radian);

  matrix3x2_t m;
  // clang-format off
  m.m11 = 1.F; m.m12 = sk;
  m.m21 = 0.F; m.m22 = 1.F;
  m.m31 = 0.F; m.m32 = 0.F;
  // clang-format on
  return m;
}

static inline matrix3x2_t _SIMD_CALLCONV matrix3x2_skewy_offcenter(float32_t radian, float32x2_t center) {
  float32_t sk = tanf(radian);
  float32_t t = -center.y * sk;

  matrix3x2_t m;
  // clang-format off
  m.m11 = 1.F; m.m12 = sk;
  m.m21 = 0.F; m.m22 = 1.F;
  m.m31 = 0.F; m.m32 = t;
  // clang-format on
  return m;
}

static inline matrix3x2_t _SIMD_CALLCONV matrix3x2_skew(float32_t radianX, float32_t radianY) {
  float32_t skx = tanf(radianX);
  float32_t sky = tanf(radianY);

  matrix3x2_t m;
  // clang-format off
  m.m11 = 1.F; m.m12 = sky;
  m.m21 = skx;  m.m22 = 1.F;
  m.m31 = 0.F; m.m32 = 0.F;
  // clang-format on
  return m;
}

static inline matrix3x2_t _SIMD_CALLCONV matrix3x2_skew_offcenter(float32_t radianX, float32_t radianY, float32x2_t center) {
  float32_t skx = tanf(radianX);
  float32_t sky = tanf(radianY);
  float32_t tx = -center.x * skx;
  float32_t ty = -center.y * sky;

  matrix3x2_t m;
  // clang-format off
  m.m11 = 1.F; m.m12 = sky;
  m.m21 = skx; m.m22 = 1.F;
  m.m31 = tx;  m.m32 = ty;
  // clang-format on
  return m;
}

// ---
// Rotation
// ---
static inline matrix3x2_t _SIMD_CALLCONV matrix3x2_rot(float32_t radian) {
  float32_t c = cosf(radian);
  float32_t s = sinf(radian);

  matrix3x2_t m;
  // clang-format off
  m.m11 =  c;  m.m12 = s;
  m.m21 = -s;  m.m22 = c;
  m.m31 = 0.F; m.m32 = 0.F;
  // clang-format on
  return m;
}

// ---
// Arithmetic
// ---
static inline matrix3x2_t _SIMD_CALLCONV matrix3x2_add(matrix3x2_t a, matrix3x2_t b) {
  matrix3x2_t m;
  m.m11 = a.m11 + b.m11;
  m.m12 = a.m12 + b.m12;
  m.m21 = a.m21 + b.m21;
  m.m22 = a.m22 + b.m22;
  m.m31 = a.m31 + b.m31;
  m.m32 = a.m32 + b.m32;
  return m;
}

static inline matrix3x2_t _SIMD_CALLCONV matrix3x2_mul(matrix3x2_t a, matrix3x2_t b) {
  matrix3x2_t m;
  m.m11 = a.m11 * b.m11 + a.m12 * b.m21;
  m.m12 = a.m11 * b.m12 + a.m12 * b.m22;
  m.m21 = a.m21 * b.m11 + a.m22 * b.m21;
  m.m22 = a.m21 * b.m12 + a.m22 * b.m22;
  m.m31 = a.m31 * b.m11 + a.m32 * b.m21 + b.m31;
  m.m32 = a.m31 * b.m12 + a.m32 * b.m22 + b.m32;
  return m;
}

static inline float32_t _SIMD_CALLCONV matrix3x2_det(matrix3x2_t a) {
  return a.m11 * a.m22 - a.m12 * a.m21;
}

// ---
// Transform
// ---
static inline int8x2_t _SIMD_CALLCONV matrix3x2_transform_int8(int8x2_t a, matrix3x2_t m) {
  int8x2_t ret;
  ret.x = (int8_t)(m.m11 * a.x + m.m21 * a.y + m.m31 + 0.5);
  ret.y = (int8_t)(m.m12 * a.x + m.m22 * a.y + m.m32 + 0.5);
  return ret;
}

static inline int16x2_t _SIMD_CALLCONV matrix3x2_transform_int16(int16x2_t a, matrix3x2_t m) {
  int16x2_t ret;
  ret.x = (int16_t)(m.m11 * a.x + m.m21 * a.y + m.m31 + 0.5);
  ret.y = (int16_t)(m.m12 * a.x + m.m22 * a.y + m.m32 + 0.5);
  return ret;
}

static inline int32x2_t _SIMD_CALLCONV matrix3x2_transform_int32(int32x2_t a, matrix3x2_t m) {
  int32x2_t ret;
  ret.x = (int32_t)(m.m11 * a.x + m.m21 * a.y + m.m31 + 0.5);
  ret.y = (int32_t)(m.m12 * a.x + m.m22 * a.y + m.m32 + 0.5);
  return ret;
}

static inline uint8x2_t _SIMD_CALLCONV matrix3x2_transform_uint8(uint8x2_t a, matrix3x2_t m) {
  uint8x2_t ret;
  ret.x = (uint8_t)(m.m11 * a.x + m.m21 * a.y + m.m31 + 0.5);
  ret.y = (uint8_t)(m.m12 * a.x + m.m22 * a.y + m.m32 + 0.5);
  return ret;
}

static inline uint16x2_t _SIMD_CALLCONV matrix3x2_transform_uint16(uint16x2_t a, matrix3x2_t m) {
  uint16x2_t ret;
  ret.x = (uint16_t)(m.m11 * a.x + m.m21 * a.y + m.m31 + 0.5);
  ret.y = (uint16_t)(m.m12 * a.x + m.m22 * a.y + m.m32 + 0.5);
  return ret;
}

static inline uint32x2_t _SIMD_CALLCONV matrix3x2_transform_uint32(uint32x2_t a, matrix3x2_t m) {
  uint32x2_t ret;
  ret.x = (uint32_t)(m.m11 * a.x + m.m21 * a.y + m.m31 + 0.5);
  ret.y = (uint32_t)(m.m12 * a.x + m.m22 * a.y + m.m32 + 0.5);
  return ret;
}

static inline float32x2_t _SIMD_CALLCONV matrix3x2_transform_float32(float32x2_t a, matrix3x2_t m) {
  float32x2_t ret;
  ret.x = m.m11 * a.x + m.m21 * a.y + m.m31;
  ret.y = m.m12 * a.x + m.m22 * a.y + m.m32;
  return ret;
}

#ifdef __cplusplus
}
#endif

#endif  // _SIMD_MATRIX3X2_H
