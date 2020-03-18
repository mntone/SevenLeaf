#ifndef _SIMD_FLOAT32X4_H
#define _SIMD_FLOAT32X4_H

#include "simd_base.h"

#ifdef __cplusplus
extern "C" {
#endif

// ---
// x86/x86-64
// ---
#ifdef _SIMD_X86

#ifdef _SIMD_X86_AVX
#define _simd_mm_shuffle_ps(__a, __m) _mm_permute_ps(__a, __m);
#else
#define _simd_mm_shuffle_ps(__a, __m) _mm_shuffle_ps(__a, __a, __m);
#endif

static inline __m128 _SIMD_CALLCONV _simd_mm_not_ps(__m128 __a) {
  return _mm_andnot_ps(__a, _mm_set1_ps(0xFFFFFFFF));
}

static inline __m128 _SIMD_CALLCONV _simd_mm_sel_ps(__m128 __a, __m128 __b, __m128 __m) {
  return _mm_or_ps(_mm_andnot_ps(__m, __a), _mm_and_ps(__m, __b));
}

static inline __m128 _SIMD_CALLCONV _simd_mm_neg_ps(__m128 __a) {
  return _mm_xor_ps(__a, _mm_set1_ps(-0.F));
}

static inline __m128 _SIMD_CALLCONV _simd_mm_abs_ps(__m128 __a) {
  return _mm_andnot_ps(__a, _mm_set1_ps(-0.F));
}

static inline __m128 _SIMD_CALLCONV _simd_mm_addsub_ps(__m128 __a, __m128 __b) {
#if defined(_SIMD_X86_SSE3)
  return _mm_addsub_ps(__a, __b);
#else
  return _mm_add_ps(__a, _mm_xor_ps(__b, _mm_set_ps(-0.F, 0.F, -0.F, 0.F)));
#endif
}

static inline __m128 _SIMD_CALLCONV _simd_mm_subadd_ps(__m128 __a, __m128 __b) {
  return _mm_add_ps(__a, _mm_xor_ps(__b, _mm_set_ps(0.F, -0.F, 0.F, -0.F)));
}

static inline __m128 _SIMD_CALLCONV _simd_mm_hadd_ps(__m128 __a) {
  float32x4_t shuffle = _mm_shuffle_ps(__a, __a, _MM_SHUFFLE(2, 3, 0, 1));  // t2, t3, t0, t1
  __a = _mm_add_ps(__a, shuffle);                                           // t3+t2, t3+t2, t1+t0, t1+t0
  shuffle = _mm_shuffle_ps(__a, __a, _MM_SHUFFLE(1, 0, 3, 2));              // t1+t0, t1+t0, t3+t2, t3+t2
  __a = _mm_add_ps(__a, shuffle);                                           // t0+t1+t2+t3, ...
  return __a;
}

static inline __m128 _SIMD_CALLCONV _simd_mm_dp_ps(__m128 __a, __m128 __b) {
#if defined(_SIMD_X86_SSE4_1)
  return _mm_dp_ps(__a, __b, 0xFF);
#else
  // NOTE: https://stackoverflow.com/a/4121295
  return _simd_mm_hadd_ps(_mm_mul_ps(__a, __b));
#endif
}

static inline float _SIMD_CALLCONV _simd_mm_dp_ps_asscalar(__m128 __a, __m128 __b) {
#if defined(_SIMD_X86_SSE4_1)
  return _mm_cvtss_f32(_mm_dp_ps(__a, __b, 0xF1));
#else
  // NOTE: https://stackoverflow.com/a/4121295
  float32x4_t t = _mm_mul_ps(__a, __b);
  float32x4_t shuffle = _mm_shuffle_ps(t, t, _MM_SHUFFLE(2, 3, 0, 1));  // t2, t3, t0, t1
  t = _mm_add_ps(t, shuffle);                                           // t3+t2, t2+t3, t1+t0, t0+t1
  t = _mm_add_ss(t, _mm_movehl_ps(shuffle, t));                         // t2, t3, t3+t2, t2+t3  -> _, _, _, t0+t1+t2+t3
  return _mm_cvtss_f32(t);
#endif
}

// ---
// ARMv7+ NEON
// ---
#elif defined(_SIMD_ARM_NEON)

static inline uint32x4_t _SIMD_CALLCONV _simd_vceqq_f32_f32(float32x4_t __p0, float32x4_t __p1) {
  return vreinterpretq_f32_u32(vceqq_f32(__p0, __p1));
}

static inline uint32x4_t _SIMD_CALLCONV _simd_vcneq_f32(float32x4_t __p0, float32x4_t __p1) {
  return vmvnq_u32(vceqq_f32(__p0, __p1));
}

static inline float32x4_t _SIMD_CALLCONV _simd_vcneq_f32_f32(float32x4_t __p0, float32x4_t __p1) {
  return vreinterpretq_f32_u32(vmvnq_u32(vceqq_f32(__p0, __p1)));
}

static inline float32x4_t _SIMD_CALLCONV _simd_vinvq_f32(float32x4_t __p0) {
  // Newton Raphson
  float32x4_t reciprocal = vrecpeq_f32(__p0);
  reciprocal = vmulq_f32(vrecpsq_f32(__p0, reciprocal), reciprocal);
  reciprocal = vmulq_f32(vrecpsq_f32(__p0, reciprocal), reciprocal);
  return reciprocal;
}

static inline float32x4_t _SIMD_CALLCONV _simd_vrsqrtq_f32(float32x4_t __p0) {
  // Newton Raphson
  float32x4_t reciprocal = vrsqrteq_f32(__p0);
  reciprocal = vmulq_f32(vrsqrtsq_f32(vmulq_f32(reciprocal, reciprocal), __p0), reciprocal);
  reciprocal = vmulq_f32(vrsqrtsq_f32(vmulq_f32(reciprocal, reciprocal), __p0), reciprocal);
  return reciprocal;
}

#ifndef _SIMD_ARM64
static inline float32x4_t _SIMD_CALLCONV vdivq_f32(float32x4_t a, float32x4_t b) {
  return vmulq_f32(a, _simd_vinvq_f32(b));
}

static inline float32x4_t _SIMD_CALLCONV vsqrtq_f32(float32x4_t __p0) {
  return vmulq_f32(__p0, _simd_vrsqrtq_f32(__p0));
}
#endif

static inline float32x4_t _SIMD_CALLCONV _simd_vdpq_f32(float32x4_t __p0) {
  float32x4_t prod = vmulq_f32(a, b);
  prod = vaddvq_f32(vaddq_f32(prod, vaddq_f32(prod, prod)));
  return prod;
}

static inline float32x4_t _SIMD_CALLCONV _simd_vmvnq_f32(float32x4_t a, float32x4_t b) {
  return vreinterpretq_f32_u32(vmvnq_u32(vreinterpretq_u32_f32(a), vreinterpretq_u32_f32(b)));
}

static inline float32x4_t _SIMD_CALLCONV _simd_vandq_f32(float32x4_t a, float32x4_t b) {
  return vreinterpretq_f32_u32(vandq_u32(vreinterpretq_u32_f32(a), vreinterpretq_u32_f32(b)));
}

static inline float32x4_t _SIMD_CALLCONV _simd_vbicq_f32(float32x4_t a, float32x4_t b) {
  return vreinterpretq_f32_u32(vbicq_u32(vreinterpretq_u32_f32(a), vreinterpretq_u32_f32(b)));
}

static inline float32x4_t _SIMD_CALLCONV _simd_vorrq_f32(float32x4_t a, float32x4_t b) {
  return vreinterpretq_f32_u32(vorrq_u32(vreinterpretq_u32_f32(a), vreinterpretq_u32_f32(b)));
}

static inline float32x4_t _SIMD_CALLCONV _simd_veorq_f32(float32x4_t __p1, float32x4_t __p1) {
  return vreinterpretq_f32_u32(veorq_u32(vreinterpretq_u32_f32(__p0), vreinterpretq_u32_f32(__p1)));
}

static inline float32x4_t _SIMD_CALLCONV _simd_vbslq_f32(float32x4_t __p0, float32x4_t __p1, float32x4_t __p2) {
  return vreinterpretq_f32_u32(
      vbslq_u32(vreinterpretq_u32_f32(__p0), vreinterpretq_u32_f32(__p1), vreinterpretq_u32_f32(__p2)));
}

#endif

// ---
// Constants
// ---
// clang-format off
#define FLOAT32X4_NEGONE     float32x4_inits(-1.F)
#define FLOAT32X4_NEGHALF    float32x4_inits(-.5F)
#define FLOAT32X4_NEGQUARTER float32x4_inits(-.25F)
#define FLOAT32X4_NEGZERO    float32x4_inits(-0.F)
#define FLOAT32X4_ZERO       float32x4_inits(0.F)
#define FLOAT32X4_QUARTER    float32x4_inits(.25F)
#define FLOAT32X4_HALF       float32x4_inits(.5F)
#define FLOAT32X4_ONE        float32x4_inits(1.F)
#define FLOAT32X4_TWO        float32x4_inits(2.F)
#define FLOAT32X4_QUARTERPI  float32x4_inits(M_PI_4)
#define FLOAT32X4_HALFPI     float32x4_inits(M_PI_2)
#define FLOAT32X4_PI         float32x4_inits(M_PI)
#define FLOAT32X4_SQRT2      float32x4_inits(M_SQRT2)
#define FLOAT32X4_RCPSQRT2   float32x4_inits(M_SQRT1_2)
#define FLOAT32X4_INF        float32x4_inits(0x7F800000)
#define FLOAT32X4_MIN        float32x4_inits(FLT_MIN)
#define FLOAT32X4_MAX        float32x4_inits(FLT_MAX)
// clang-format on

// ---
// Inits
// ---
static inline float32x4_t _SIMD_CALLCONV float32x4_initv(float x, float y, float z, float w) {
  float32x4_t ret;
#if defined(_SIMD_ARM_NEON)
  ret = { x, y, z, w };
#elif defined(_SIMD_X86_SSE)
  ret = _mm_setr_ps(x, y, z, w);
#else
  ret.f32[0] = x;
  ret.f32[1] = y;
  ret.f32[2] = z;
  ret.f32[3] = w;
#endif
  return ret;
}

static inline float32x4_t _SIMD_CALLCONV float32x4_inits(float s) {
  float32x4_t ret;
#if defined(_SIMD_ARM_NEON)
  ret = vdupq_n_f32(s);
#elif defined(_SIMD_X86_SSE)
  ret = _mm_set1_ps(s);
#else
  for (size_t i = 0; i < 4; ++i) {
    ret.f32[i] = s;
  }
#endif
  return ret;
}

static inline float32x4_t _SIMD_CALLCONV float32x4_inita(const float a[]) {
  float32x4_t ret;
#if defined(_SIMD_ARM_NEON)
  ret = vld1q_f32(a);
#elif defined(_SIMD_X86_SSE)
  ret = _mm_load_ps(a);
#else
  for (size_t i = 0; i < 4; ++i) {
    ret.f32[i] = a[i];
  }
#endif
  return ret;
}

static inline float32x4_t _SIMD_CALLCONV float32x4_initp(const float *p) {
  float32x4_t ret;
#if defined(_SIMD_ARM_NEON)
  ret = vld1q_dup_f32(p);
#elif defined(_SIMD_X86_AVX)
  ret = _mm_broadcast_ss(p);
#elif defined(_SIMD_X86_SSE)
  ret = _mm_load_ps1(p);
#else
  float s = *p;
  for (size_t i = 0; i < 4; ++i) {
    ret.f32[i] = s;
  }
#endif
  return ret;
}

// ---
// Gets
// ---
static inline float _SIMD_CALLCONV float32x4_getx(float32x4_t v) {
#if defined(_SIMD_ARM_NEON)
  return vgetq_lane_f32(v, 0);
#elif defined(_SIMD_X86_SSE)
  return _mm_cvtss_f32(v);
#else
  return v.f32[0];
#endif
}

static inline float _SIMD_CALLCONV float32x4_gety(float32x4_t v) {
#if defined(_SIMD_ARM_NEON)
  return vgetq_lane_f32(v, 1);
#elif defined(_SIMD_X86_SSE4_1)
  int ret = _mm_extract_ps(v, 1);
  return *(float *)&ret;
#elif defined(_SIMD_X86_SSE)
  return _mm_cvtss_f32(_simd_mm_shuffle_ps(v, _MM_SHUFFLE(1, 1, 1, 1)));
#else
  return v.f32[1];
#endif
}

static inline float _SIMD_CALLCONV float32x4_getz(float32x4_t v) {
#if defined(_SIMD_ARM_NEON)
  return vgetq_lane_f32(v, 2);
#elif defined(_SIMD_X86_SSE4_1)
  int ret = _mm_extract_ps(v, 2);
  return *(float *)&ret;
#elif defined(_SIMD_X86_SSE)
  return _mm_cvtss_f32(_simd_mm_shuffle_ps(v, _MM_SHUFFLE(2, 2, 2, 2)));
#else
  return v.f32[2];
#endif
}

static inline float _SIMD_CALLCONV float32x4_getw(float32x4_t v) {
#if defined(_SIMD_ARM_NEON)
  return vgetq_lane_f32(v, 3);
#elif defined(_SIMD_X86_SSE4_1)
  int ret = _mm_extract_ps(v, 3);
  return *(float *)&ret;
#elif defined(_SIMD_X86_SSE)
  return _mm_cvtss_f32(_simd_mm_shuffle_ps(v, _MM_SHUFFLE(3, 3, 3, 3)));
#else
  return v.f32[3];
#endif
}

static inline void _SIMD_CALLCONV float32x4_getxp(float32x4_t v, float *p) {
#if defined(_SIMD_ARM_NEON)
  vst1q_lane_f32(p, v, 0);
#elif defined(_SIMD_X86_SSE)
  _mm_store_ss(p, v);
#else
  *p = v.f32[0];
#endif
}

static inline void _SIMD_CALLCONV float32x4_getyp(float32x4_t v, float *p) {
#if defined(_SIMD_ARM_NEON)
  vst1q_lane_f32(p, v, 1);
#elif defined(_SIMD_X86_SSE4_1)
  *((int *)p) = _mm_extract_ps(v, 1);
#elif defined(_SIMD_X86_SSE)
  _mm_store_ss(p, _simd_mm_shuffle_ps(v, _MM_SHUFFLE(1, 1, 1, 1)));
#else
  *p = v.f32[1];
#endif
}

static inline void _SIMD_CALLCONV float32x4_getzp(float32x4_t v, float *p) {
#if defined(_SIMD_ARM_NEON)
  vst1q_lane_f32(p, v, 2);
#elif defined(_SIMD_X86_SSE4_1)
  *((int *)p) = _mm_extract_ps(v, 2);
#elif defined(_SIMD_X86_SSE)
  _mm_store_ss(p, _simd_mm_shuffle_ps(v, _MM_SHUFFLE(2, 2, 2, 2)));
#else
  *p = v.f32[2];
#endif
}

static inline void _SIMD_CALLCONV float32x4_getwp(float32x4_t v, float *p) {
#if defined(_SIMD_ARM_NEON)
  vst1q_lane_f32(p, v, 3);
#elif defined(_SIMD_X86_SSE4_1)
  *((int *)p) = _mm_extract_ps(v, 3);
#elif defined(_SIMD_X86_SSE)
  _mm_store_ss(p, _simd_mm_shuffle_ps(v, _MM_SHUFFLE(3, 3, 3, 3)));
#else
  *p = v.f32[3];
#endif
}

// ---
// Sets
// ---
static inline float32x4_t _SIMD_CALLCONV float32x4_setx(float32x4_t v, float x) {
  float32x4_t ret;
#if defined(_SIMD_ARM_NEON)
  ret = vsetq_lane_f32(x, v, 0);
#elif defined(_SIMD_X86_SSE)
  ret = _mm_move_ss(v, _mm_set_ss(x));
#else
  ret.f32[0] = x;
#endif
  return ret;
}

static inline float32x4_t _SIMD_CALLCONV float32x4_sety(float32x4_t v, float y) {
  float32x4_t ret;
#if defined(_SIMD_ARM_NEON)
  ret = vsetq_lane_f32(y, v, 1);
#elif defined(_SIMD_X86_SSE4_1)
  ret = _mm_insert_ps(v, _mm_set_ss(y), 0x10);
#elif defined(_SIMD_X86_SSE)
  ret = _simd_mm_permute_ps(v, _MM_SHUFFLE(3, 2, 0, 1));
  ret = _mm_move_ss(ret, _mm_set_ss(y));
  ret = _simd_mm_permute_ps(ret, _MM_SHUFFLE(3, 2, 0, 1));
#else
  ret.f32[1] = y;
#endif
  return ret;
}

static inline float32x4_t _SIMD_CALLCONV float32x4_setz(float32x4_t v, float z) {
  float32x4_t ret;
#if defined(_SIMD_ARM_NEON)
  ret = vsetq_lane_f32(z, v, 2);
#elif defined(_SIMD_X86_SSE4_1)
  ret = _mm_insert_ps(v, _mm_set_ss(z), 0x20);
#elif defined(_SIMD_X86_SSE)
  ret = _simd_mm_shuffle_ps(v, _MM_SHUFFLE(3, 0, 1, 2));
  ret = _mm_move_ss(ret, _mm_set_ss(z));
  ret = _simd_mm_shuffle_ps(ret, _MM_SHUFFLE(3, 0, 1, 2));
#else
  ret.f32[2] = z;
#endif
  return ret;
}

static inline float32x4_t _SIMD_CALLCONV float32x4_setw(float32x4_t v, float w) {
  float32x4_t ret;
#if defined(_SIMD_ARM_NEON)
  ret = vsetq_lane_f32(w, v, 3);
#elif defined(_SIMD_X86_SSE4_1)
  ret = _mm_insert_ps(v, _mm_set_ss(w), 0x30);
#elif defined(_SIMD_X86_SSE)
  ret = _simd_mm_shuffle_ps(v, _MM_SHUFFLE(0, 2, 1, 3));
  ret = _mm_move_ss(ret, _mm_set_ss(w));
  ret = _simd_mm_shuffle_ps(ret, _MM_SHUFFLE(0, 2, 1, 3));
#else
  ret.f32[3] = w;
#endif
  return ret;
}

// ---
// Packing
// ---
// a0, b0, a1, b1
static inline float32x4_t _SIMD_CALLCONV float32x4_xxyy(float32x4_t a, float32x4_t b) {
  float32x4_t ret;
#if defined(_SIMD_ARM_NEON)
  ret = vzipq_f32(a, b).val[0];
#elif defined(_SIMD_X86_SSE)
  ret = _mm_unpacklo_ps(a, b);
#else
  ret.f32[0] = a.f32[0];
  ret.f32[1] = b.f32[0];
  ret.f32[2] = a.f32[1];
  ret.f32[3] = b.f32[1];
#endif
  return ret;
}

// a2, b2, a3, b3
static inline float32x4_t _SIMD_CALLCONV float32x4_zzww(float32x4_t a, float32x4_t b) {
  float32x4_t ret;
#if defined(_SIMD_ARM_NEON)
  ret = vzipq_f32(a, b).val[1];
#elif defined(_SIMD_X86_SSE)
  ret = _mm_unpackhi_ps(a, b);
#else
  ret.f32[0] = a.f32[2];
  ret.f32[1] = b.f32[2];
  ret.f32[2] = a.f32[3];
  ret.f32[3] = b.f32[3];
#endif
  return ret;
}

// a0, b0, a1, b1 | a2, b2, a3, b3
static inline float32x4x2_t _SIMD_CALLCONV float32x4_xxyy_zzww(float32x4_t a, float32x4_t b) {
  float32x4x2_t ret;
#if defined(_SIMD_ARM_NEON)
  ret = vzipq_f32(a.v, b.v);
#else
  ret.val[0] = float32x4_xxyy(a, b);
  ret.val[1] = float32x4_zzww(a, b);
#endif
  return ret;
}

// ---
// Logic
// ---
// ~a
static inline float32x4_t _SIMD_CALLCONV float32x4_not(float32x4_t a) {
  float32x4_t ret;
#if defined(_SIMD_ARM_NEON)
  ret = _simd_vmvnq_f32(a);
#elif defined(_SIMD_X86_SSE)
  ret = _simd_mm_not_ps(a);
#else
  ret = __simd128_not(a);
#endif
  return ret;
}

// a & b
static inline float32x4_t _SIMD_CALLCONV float32x4_and(float32x4_t a, float32x4_t b) {
  float32x4_t ret;
#if defined(_SIMD_ARM_NEON)
  ret = _simd_vandq_f32(a, b);
#elif defined(_SIMD_X86_SSE)
  ret = _mm_and_ps(a, b);
#else
  ret = __simd128_and(a, b);
#endif
  return ret;
}

// ~a & b
static inline float32x4_t _SIMD_CALLCONV float32x4_andnot(float32x4_t a, float32x4_t b) {
  float32x4_t ret;
#if defined(_SIMD_ARM_NEON)
  ret = _simd_vbicq_f32(b, a);
#elif defined(_SIMD_X86_SSE)
  ret = _mm_andnot_ps(a, b);
#else
  ret = __simd128_andnot(a, b);
#endif
  return ret;
}

// a | b
static inline float32x4_t _SIMD_CALLCONV float32x4_or(float32x4_t a, float32x4_t b) {
  float32x4_t ret;
#if defined(_SIMD_ARM_NEON)
  ret = _simd_vorrq_f32(a, b);
#elif defined(_SIMD_X86_SSE)
  ret = _mm_or_ps(a, b);
#else
  ret = __simd128_or(a, b);
#endif
  return ret;
}

// ~a | b
static inline float32x4_t _SIMD_CALLCONV float32x4_ornot(float32x4_t a, float32x4_t b) {
  float32x4_t ret;
#if defined(_SIMD_ARM_NEON)
  ret = _simd_vornq_f32(b, a);
#elif defined(_SIMD_X86_SSE)
  ret = _mm_or_ps(_simd_mm_not_ps(a), b);
#else
  ret = __simd128_ornot(a, b);
#endif
  return ret;
}

// a ^ b
static inline float32x4_t _SIMD_CALLCONV float32x4_xor(float32x4_t a, float32x4_t b) {
  float32x4_t ret;
#if defined(_SIMD_ARM_NEON)
  ret = _simd_veorq_f32(a, b);
#elif defined(_SIMD_X86_SSE)
  ret = _mm_xor_ps(a, b);
#else
  ret = __simd128_xor(a, b);
#endif
  return ret;
}

// (a & ~m) | (b & m)
static inline float32x4_t _SIMD_CALLCONV float32x4_sel(float32x4_t a, float32x4_t b, float32x4_t m) {
  float32x4_t ret;
#if defined(_SIMD_ARM_NEON)
  ret = _simd_vbslq_f32(m, a, b);
#elif defined(_SIMD_X86_SSE)
  ret = _simd_mm_sel_ps(a, b, m);
#else
  ret = __simd128_sel(a, b, m);
#endif
  return ret;
}

// ---
// Arithmetic
// ---
// -a
static inline float32x4_t _SIMD_CALLCONV float32x4_neg(float32x4_t a) {
  float32x4_t ret;
#if defined(_SIMD_ARM_NEON)
  ret = vnegq_f32(a);
#elif defined(_SIMD_X86_SSE)
  ret = _simd_mm_neg_ps(a);
#else
  for (size_t i = 0; i < 4; ++i) {
    ret.f32[i] = -a.f32[i];
  }
#endif
  return ret;
}

// a + b
static inline float32x4_t _SIMD_CALLCONV float32x4_add(float32x4_t a, float32x4_t b) {
  float32x4_t ret;
#if defined(_SIMD_ARM_NEON)
  ret = vaddq_f32(a, b);
#elif defined(_SIMD_X86_SSE)
  ret = _mm_add_ps(a, b);
#else
  for (size_t i = 0; i < 4; ++i) {
    ret.f32[i] = a.f32[i] + b.f32[i];
  }
#endif
  return ret;
}

// a - b
static inline float32x4_t _SIMD_CALLCONV float32x4_sub(float32x4_t a, float32x4_t b) {
  float32x4_t ret;
#if defined(_SIMD_ARM_NEON)
  ret = vsubq_f32(a, b);
#elif defined(_SIMD_X86_SSE)
  ret = _mm_sub_ps(a, b);
#else
  for (size_t i = 0; i < 4; ++i) {
    ret.f32[i] = a.f32[i] - b.f32[i];
  }
#endif
  return ret;
}

// a0 + b0, a1 - b1, a2 + b2, a3 - b3
static inline float32x4_t _SIMD_CALLCONV float32x4_addsub(float32x4_t a, float32x4_t b) {
  float32x4_t ret;
#if defined(_SIMD_ARM_NEON)
  float32x4_t v = { 0.F, -0.F, 0.F, -0.F };
  ret = vaddq_f32(a, veorq_f32(vld1_f32(v), b));
#elif defined(_SIMD_X86_SSE)
  ret = _simd_mm_addsub_ps(a, b);
#else
  for (size_t i = 0; i < 4; i += 2) {
    ret.f32[i] = a.f32[i] + b.f32[i];
    ret.f32[i + 1] = a.f32[i + 1] - b.f32[i + 1];
  }
#endif
  return ret;
}

// a0 - b0, a1 + b1, a2 - b2, a3 + b3
static inline float32x4_t _SIMD_CALLCONV float32x4_subadd(float32x4_t a, float32x4_t b) {
  float32x4_t ret;
#if defined(_SIMD_ARM_NEON)
  float32x4_t v = { -0.F, 0.F, -0.F, 0.F };
  ret = vaddq_f32(a.v, veorq_f32(vld1_f32(v), b));
#elif defined(_SIMD_X86_SSE)
  ret = _simd_mm_subadd_ps(a, b);
#else
  for (size_t i = 0; i < 4; i += 2) {
    ret.f32[i] = a.f32[i] - b.f32[i];
    ret.f32[i + 1] = a.f32[i + 1] + b.f32[i + 1];
  }
#endif
  return ret;
}

// a0 + a1, a2 + a3, b0 + b1, b1 + b3
static inline float32x4_t _SIMD_CALLCONV float32x4_hadd(float32x4_t a, float32x4_t b) {
  float32x4_t ret;
#if defined(_SIMD_ARM_NEON)
  ret = vpaddq_f32(a, b);
#elif defined(_SIMD_X86_SSE3)
  ret = _mm_hadd_ps(a, b);
#else
  ret.f32[0] = a.f32[0] + a.f32[1];
  ret.f32[1] = a.f32[2] + a.f32[3];
  ret.f32[2] = b.f32[0] + b.f32[1];
  ret.f32[3] = b.f32[2] + b.f32[3];
#endif
  return ret;
}

// a0 - a1, a2 - a3, b0 - b1, b1 - b3
static inline float32x4_t _SIMD_CALLCONV float32x4_hsub(float32x4_t a, float32x4_t b) {
  float32x4_t ret;
#if defined(_SIMD_ARM_NEON)
  ret = vpaddq_f32(a, vnegq_f32(b));
#elif defined(_SIMD_X86_SSE3)
  ret = _mm_hsub_ps(a, b);
#else
  ret.f32[0] = a.f32[0] - a.f32[1];
  ret.f32[1] = a.f32[2] - a.f32[3];
  ret.f32[2] = b.f32[0] - b.f32[1];
  ret.f32[3] = b.f32[2] - b.f32[3];
#endif
  return ret;
}

// a * b
static inline float32x4_t _SIMD_CALLCONV float32x4_mul(float32x4_t a, float32x4_t b) {
  float32x4_t ret;
#if defined(_SIMD_ARM_NEON)
  ret = vmulq_f32(a, b);
#elif defined(_SIMD_X86_SSE)
  ret = _mm_mul_ps(a, b);
#else
  for (size_t i = 0; i < 4; ++i) {
    ret.f32[i] = a.f32[i] * b.f32[i];
  }
#endif
  return ret;
}

// a / b
static inline float32x4_t _SIMD_CALLCONV float32x4_div(float32x4_t a, float32x4_t b) {
  float32x4_t ret;
#if defined(_SIMD_ARM_NEON)
  ret = vdivq_f32(a, b);
#elif defined(_SIMD_X86_SSE)
  ret = _mm_div_ps(a, b);
#else
  for (size_t i = 0; i < 4; ++i) {
    ret.f32[i] = a.f32[i] / b.f32[i];
  }
#endif
  return ret;
}

// a * s
static inline float32x4_t _SIMD_CALLCONV float32x4_scale(float32x4_t a, float s) {
  float32x4_t ret;
#if defined(_SIMD_ARM_NEON)
  ret = vmulq_n_f32(a, s);
#elif defined(_SIMD_X86_SSE)
  ret = _mm_mul_ps(a, _mm_set1_ps(s));
#else
  for (size_t i = 0; i < 4; ++i) {
    ret.f32[i] = a.f32[i] * s;
  }
#endif
  return ret;
}

// 1 / a estimated
static inline float32x4_t _SIMD_CALLCONV float32x4_rcp_est(float32x4_t a) {
  float32x4_t ret;
#if defined(_SIMD_ARM_NEON)
  ret = vrecpsq_f32(a);  // 8-bit
#elif defined(_SIMD_X86_SSE)
  ret = _mm_rcp_ps(a);  // 11-bit
#else
  for (size_t i = 0; i < 4; ++i) {
    ret.f32[i] = 1.F / a.f32[i];
  }
#endif
  return ret;
}

// 1 / a
static inline float32x4_t _SIMD_CALLCONV float32x4_rcp(float32x4_t a) {
  float32x4_t ret;
#if defined(_SIMD_ARM_NEON)
  ret = _simd_vinvq_f32(a);
#elif defined(_SIMD_X86_SSE)
  // Newton Raphson
  float32x4_t reciprocal = _mm_rcp_ps(a);
  float32x4_t muls = _mm_mul_ps(_mm_mul_ps(a, reciprocal), reciprocal);
  ret = _mm_sub_ps(_mm_add_ps(reciprocal, reciprocal), muls);
#else
  for (size_t i = 0; i < 4; ++i) {
    ret.f32[i] = 1.F / a.f32[i];
  }
#endif
  return ret;
}

// sqrt(a) estimated
static inline float32x4_t _SIMD_CALLCONV float32x4_sqrt_est(float32x4_t a) {
  float32x4_t ret;
#if defined(_SIMD_ARM_NEON)
  // Newton Raphson
  float32x4_t reciprocal = vrsqrteq_f32(a);
  reciprocal = vmulq_f32(vrsqrtsq_f32(vmulq_f32(reciprocal, reciprocal), a), reciprocal);
  ret = vmulq_f32(a, reciprocal);
#elif defined(_SIMD_X86_SSE)
  ret = _mm_mul_ps(a, _mm_rsqrt_ps(a));
#else
  for (size_t i = 0; i < 4; ++i) {
    ret.f32[i] = sqrtf(a.f32[i]);
  }
#endif
  return ret;
}

// sqrt(a)
static inline float32x4_t _SIMD_CALLCONV float32x4_sqrt(float32x4_t a) {
  float32x4_t ret;
#if defined(_SIMD_ARM_NEON)
  ret = vsqrtq_f32(a);
#elif defined(_SIMD_X86_SSE)
  ret = _mm_sqrt_ps(a);
#else
  for (size_t i = 0; i < 4; ++i) {
    ret.f32[i] = sqrtf(a.f32[i]);
  }
#endif
  return ret;
}

// 1 / sqrt(a) estimated
static inline float32x4_t _SIMD_CALLCONV float32x4_invsqrt_est(float32x4_t a) {
  float32x4_t ret;
#if defined(_SIMD_ARM_NEON)
  ret = vrsqrteq_f32(a);  // 8-bit
#elif defined(_SIMD_X86_SSE)
  ret = _mm_rsqrt_ps(a);  // 11-bit
#else
  for (size_t i = 0; i < 4; ++i) {
    ret.f32[i] = 1.F / sqrtf(a.f32[i]);
  }
#endif
  return ret;
}

// 1 / sqrt(a)
static inline float32x4_t _SIMD_CALLCONV float32x4_invsqrt(float32x4_t a) {
  float32x4_t ret;
#if defined(_SIMD_ARM_NEON)
  ret = _simd_vrsqrtq_f32(a);
#elif defined(_SIMD_X86) && defined(_SIMD_X86_SSE)
  // Newton Raphson
  float32x4_t reciprocal = _mm_rsqrt_ps(a);
  float32x4_t ahalf = _mm_mul_ps(a, _mm_set1_ps(-.5F));
#ifdef _SIMD_X86_FMA3
  float32x4_t muls1 = _mm_mul_ps(ahalf, reciprocal);
  ret = _mm_mul_ps(_mm_fmadd_ps(muls1, reciprocal, _mm_set1_ps(1.5F)), reciprocal);
#else
  float32x4_t muls2 = _mm_mul_ps(_mm_mul_ps(ahalf, reciprocal), reciprocal);
  ret = _mm_mul_ps(_mm_add_ps(muls2, _mm_set1_ps(1.5F)), reciprocal);
#endif
#else
  for (size_t i = 0; i < 4; ++i) {
    ret.f32[i] = 1.F / sqrtf(a.f32[i]);
  }
#endif
  return ret;
}

// abs(a)
static inline float32x4_t _SIMD_CALLCONV float32x4_abs(float32x4_t a) {
  float32x4_t ret;
#if defined(_SIMD_ARM_NEON)
  ret = vabsq_f32(a);
#elif defined(_SIMD_X86_SSE)
  ret = _simd_mm_abs_ps(a);
#else
  for (size_t i = 0; i < 4; ++i) {
    ret.f32[i] = fabsf(a.f32[i]);
  }
#endif
  return ret;
}

// abs(a - b)
static inline float32x4_t _SIMD_CALLCONV float32x4_subabs(float32x4_t a, float32x4_t b) {
  float32x4_t ret;
#if defined(_SIMD_ARM_NEON)
  ret = vabdq_f32(a, b);
#elif defined(_SIMD_X86_SSE)
  ret = _simd_mm_abs_ps(_mm_sub_ps(a, b));
#else
  for (size_t i = 0; i < 4; ++i) {
    ret.f32[i] = fabsf(a.f32[i] - b.f32[i]);
  }
#endif
  return ret;
}

// |a|^2
static inline float _SIMD_CALLCONV float32x4_lensq(float32x4_t a) {
#if defined(_SIMD_ARM_NEON)
  float32x4_t prod = vmulq_f32(a, a);
  prod = vaddvq_f32(vaddq_f32(prod, vaddq_f32(prod, prod)));
  return vgetq_lane_f32(prod, 0);
#elif defined(_SIMD_X86_SSE)
  return _simd_mm_dp_ps_asscalar(a, a);
#else
  float32x4_t muls = float32x4_mul(a, a);
  return muls.f32[0] + muls.f32[1] + muls.f32[2] + muls.f32[3];
#endif
}

// |a|
static inline float _SIMD_CALLCONV float32x4_len(float32x4_t a) {
  return sqrtf(float32x4_lensq(a));
}

// |a - b|^2
static inline float _SIMD_CALLCONV float32x4_distsq(float32x4_t a, float32x4_t b) {
  float32x4_t d = float32x4_sub(a, b);
  return float32x4_lensq(d);
}

// |a - b|
static inline float _SIMD_CALLCONV float32x4_dist(float32x4_t a, float32x4_t b) {
  return sqrtf(float32x4_distsq(a, b));
}

// a•b
static inline float32x4_t _SIMD_CALLCONV float32x4_dot(float32x4_t a, float32x4_t b) {
  float32x4_t ret;
#if defined(_SIMD_ARM_NEON)
  ret = _simd_vdpq_f32(a, b);
#elif defined(_SIMD_X86_SSE)
  ret = _simd_mm_dp_ps(a, b);
#else
  ret.f32[0] = ret.f32[1] = ret.f32[2] = ret.f32[3] = float32x4_lensq(a);
#endif
  return ret;
}

// a / |a|
static inline float32x4_t _SIMD_CALLCONV float32x4_normalize(float32x4_t a) {
  float32x4_t ret;
#if defined(_SIMD_ARM_NEON)
  float32x4_t lensq = _simd_vdpq_f32(a, b);
  float32x4_t len = vsqrtq_f32(lensq);
  __m128 mask = _simd_vorrq_f32(_simd_vcneq_f32_f32(len, vdupq_n_f32(0.F)),
                                _simd_vcneq_f32_f32(lensq, vdupq_n_f32(0x7F800000)));  // 0/inf: 0, other: ~0
  ret = _simd_vbslq_f32(vdupq_n_f32(0x7FC00000 /* QNan */), _mm_div_ps(a, len), mask);
#elif defined(_SIMD_X86_SSE)
  __m128 lensq = _simd_mm_dp_ps(a, a);
  __m128 len = _mm_sqrt_ps(lensq);
  __m128 mask = _mm_or_ps(_mm_cmpneq_ps(len, _mm_setzero_ps()),
                          _mm_cmpneq_ps(lensq, _mm_set1_ps(0x7F800000)));  // 0/inf: 0, other: ~0
  ret = _simd_mm_sel_ps(_mm_set1_ps(0x7FC00000 /* QNan */), _mm_div_ps(a, len), mask);
#else
  float lensq = float32x4_lensq(a);
  if (lensq > 0.F) {
    float factor = 1.F / sqrtf(lensq);
    for (size_t i = 0; i < 4; ++i) {
      ret.f32[i] = a.f32[i] * factor;
    }
  } else {
    for (size_t i = 0; i < 4; ++i) {
      ret.f32[i] = 0x7FC00000 /* QNan */;
    }
  }
#endif
  return ret;
}

// ---
// FMA
// ---
// a * b + c
static inline float32x4_t _SIMD_CALLCONV float32x4_muladd(float32x4_t a, float32x4_t b, float32x4_t c) {
  float32x4_t ret;
#if defined(_SIMD_ARM_NEON)
#ifdef _SIMD_ARM64
  ret = vfmaq_f32(c, a, b);
#else
  ret = vmlaq_f32(c, a, b);
#endif
#elif defined(_SIMD_X86_FMA3)
  ret = _mm_fmadd_ps(a, b, c);
#elif defined(_SIMD_X86_SSE)
  ret = _mm_add_ps(_mm_mul_ps(a, b), c);
#else
  for (size_t i = 0; i < 4; ++i) {
    ret.f32[i] = a.f32[i] * b.f32[i] + c.f32[i];
  }
#endif
  return ret;
}

// a * b - c
static inline float32x4_t _SIMD_CALLCONV float32x4_mulsub(float32x4_t a, float32x4_t b, float32x4_t c) {
  float32x4_t ret;
#if defined(_SIMD_ARM_NEON)
#ifdef _SIMD_ARM64
  ret = vnegq_f32(vfmsq_f32(c, a, b));
#else
  ret = vnegq_f32(vmlsq_f32(c, a, b));
#endif
#elif defined(_SIMD_X86_FMA3)
  ret = _mm_fmsub_ps(a, b, c);
#elif defined(_SIMD_X86_SSE)
  ret = _mm_sub_ps(_mm_mul_ps(a, b), c);
#else
  for (size_t i = 0; i < 4; ++i) {
    ret.f32[i] = a.f32[i] * b.f32[i] - c.f32[i];
  }
#endif
  return ret;
}

// a0 * b0 + c0, a1 * b1 - c1, a2 * b2 + c2, a3 * b3 - c3
static inline float32x4_t _SIMD_CALLCONV float32x4_muladdsub(float32x4_t a, float32x4_t b, float32x4_t c) {
  float32x4_t ret;
#if defined(_SIMD_ARM_NEON)
  float32x4_t v = { 0.F, -0.F, 0.F, -0.F };
#ifdef _SIMD_ARM64
  ret = vfmaq_f32(veorq_f32(vld1_f32(v), c), a, b);
#else
  ret = vmlaq_f32(veorq_f32(vld1_f32(v), c), a, b);
#endif
#elif defined(_SIMD_X86_FMA3)
  ret = _mm_fmaddsub_ps(a, b, c);
#elif defined(_SIMD_X86_SSE)
  ret = _simd_mm_addsub_ps(_mm_mul_ps(a, b), c);
#else
  for (size_t i = 0; i < 4; i += 2) {
    ret.f32[i] = a.f32[i] * b.f32[i] + c.f32[i];
    ret.f32[i + 1] = a.f32[i + 1] * b.f32[i + 1] - c.f32[i + 1];
  }
#endif
  return ret;
}

// a0 * b0 - c0, a1 * b1 + c1, a2 * b2 - c2, a3 * b3 + c3
static inline float32x4_t _SIMD_CALLCONV float32x4_mulsubadd(float32x4_t a, float32x4_t b, float32x4_t c) {
  float32x4_t ret;
#if defined(_SIMD_ARM_NEON)
  float32x4_t v = { -0.F, 0.F, -0.F, 0.F };
#ifdef _SIMD_ARM64
  ret = vfmaq_f32(veorq_f32(vld1_f32(v), c.v), a, b);
#else
  ret = vmlaq_f32(veorq_f32(vld1_f32(v), c.v), a, b);
#endif
#elif defined(_SIMD_X86_FMA3)
  ret = _mm_fmsubadd_ps(a, b, c);
#elif defined(_SIMD_X86_SSE)
  ret = _simd_mm_subadd_ps(_mm_mul_ps(a, b), c);
#else
  for (size_t i = 0; i < 4; i += 2) {
    ret.f32[i] = a.f32[i] * b.f32[i] - c.f32[i];
    ret.f32[i + 1] = a.f32[i + 1] * b.f32[i + 1] + c.f32[i + 1];
  }
#endif
  return ret;
}

// -a * b + c
static inline float32x4_t _SIMD_CALLCONV float32x4_negmuladd(float32x4_t a, float32x4_t b, float32x4_t c) {
  float32x4_t ret;
#if defined(_SIMD_ARM_NEON)
#ifdef _SIMD_ARM64
  ret = vfmsq_f32(c, a, b);
#else
  ret = vmlsq_f32(c, a, b);
#endif
#elif defined(_SIMD_X86_FMA3)
  ret = _mm_fnmadd_ps(a, b, c);
#elif defined(_SIMD_X86_SSE)
  ret = _mm_sub_ps(c, _mm_mul_ps(a, b));
#else
  for (size_t i = 0; i < 4; ++i) {
    ret.f32[i] = -a.f32[i] * b.f32[i] + c.f32[i];
  }
#endif
  return ret;
}

// -a * b - c
static inline float32x4_t _SIMD_CALLCONV float32x4_negmulsub(float32x4_t a, float32x4_t b, float32x4_t c) {
  float32x4_t ret;
#if defined(_SIMD_ARM_NEON)
#ifdef _SIMD_ARM64
  ret = vfmsq_f32(vnegq_f32(c), a, b);
#else
  ret.v = vmlsq_f32(vnegq_f32(c), a, b);
#endif
#elif defined(_SIMD_X86_FMA3)
  ret = _mm_fnmsub_ps(a, b, c);
#elif defined(_SIMD_X86_SSE)
  ret = _mm_add_ps(_mm_mul_ps(a, b), c);
#else
  for (size_t i = 0; i < 4; ++i) {
    ret.f32[i] = -(a.f32[i] * b.f32[i] + c.f32[i]);
  }
#endif
  return ret;
}

// ---
// Compare
// ---
// isnan(a)
static inline float32x4_t _SIMD_CALLCONV float32x4_isnan(float32x4_t a) {
  float32x4_t ret;
#if defined(_SIMD_ARM_NEON)
  ret = _simd_vcneq_f32_f32(a, a);
#elif defined(_SIMD_X86_SSE)
  ret = _mm_cmpneq_ps(a, a);
#else
  for (size_t i = 0; i < 4; ++i) {
    ret.f32[i] = isnan(a.f32[i]) ? 0xFFFFFFFF : 0;
  }
#endif
  return ret;
}

// isinf(a)
static inline float32x4_t _SIMD_CALLCONV float32x4_isinf(float32x4_t a) {
  float32x4_t ret;
#if defined(_SIMD_ARM_NEON)
  uint32x4_t negzero = { -0.F, -0.F, -0.F, -0.F };
  uint32x4_t inf = { 0x7F800000, 0x7F800000, 0x7F800000, 0x7F800000 };
  ret = _simd_vceqq_f32_f32(vandq_u32(vreinterpretq_u32_f32(a), negzero), inf);
#elif defined(_SIMD_X86_SSE)
  ret = _mm_cmpeq_ps(_mm_and_ps(a, _mm_set1_ps(-0.F)), _mm_set1_ps(0x7F800000));
#else
  for (size_t i = 0; i < 4; ++i) {
    ret.f32[i] = isinf(a.f32[i]) ? 0xFFFFFFFF : 0;
  }
#endif
  return ret;
}

// [vector] min(a, b)
static inline float32x4_t _SIMD_CALLCONV float32x4_min(float32x4_t a, float32x4_t b) {
  float32x4_t ret;
#if defined(_SIMD_ARM_NEON)
  ret = vminq_f32(a, b);
#elif defined(_SIMD_X86_SSE)
  ret = _mm_min_ps(a, b);
#else
  for (size_t i = 0; i < 4; ++i) {
    ret.f32[i] = fminf(a.f32[i], b.f32[i]);
  }
#endif
  return ret;
}

// [scalar] min(a, s)
static inline float32x4_t _SIMD_CALLCONV float32x4_mins(float32x4_t a, float s) {
  return float32x4_min(a, float32x4_inits(s));
}

// hmin(a)
static inline float32x4_t _SIMD_CALLCONV float32x4_hmin(float32x4_t a) {
  float32x4_t ret;
#if defined(_SIMD_ARM_NEON)
  ret = vminq_f32(a, b);
#elif defined(_SIMD_X86_SSE)
  __m128 shuffle = _mm_shuffle_ps(a, a, _MM_SHUFFLE(2, 3, 0, 1));  // t2, t3, t0, t1
  a = _mm_min_ps(a, shuffle);                                      // min(t3, t2), min(t3, t2), min(t1, t0), min(t1, t0)
  shuffle = _mm_shuffle_ps(a, a, _MM_SHUFFLE(1, 0, 3, 2));         // min(t1, t0), min(t1, t0), min(t3, t2), min(t3, t2)
  ret = _mm_min_ps(a, shuffle);                                    // min(t3, t2, t1, t0), ...
#else
  ret.f32[0] = ret.f32[1] = ret.f32[2] = ret.f32[3] = fminf(fminf(fminf(a.f32[0], a.f32[1]), a.f32[2]), a.f32[3]);
#endif
  return ret;
}

// [vector] max(a, b)
static inline float32x4_t _SIMD_CALLCONV float32x4_max(float32x4_t a, float32x4_t b) {
  float32x4_t ret;
#if defined(_SIMD_ARM_NEON)
  ret = vmaxq_f32(a, b);
#elif defined(_SIMD_X86_SSE)
  ret = _mm_max_ps(a, b);
#else
  for (size_t i = 0; i < 4; ++i) {
    ret.f32[i] = fmaxf(a.f32[i], b.f32[i]);
  }
#endif
  return ret;
}

// [scalar] max(a, s)
static inline float32x4_t _SIMD_CALLCONV float32x4_maxs(float32x4_t a, float s) {
  return float32x4_max(a, float32x4_inits(s));
}

// hmax(a)
static inline float32x4_t _SIMD_CALLCONV float32x4_hmax(float32x4_t a) {
  float32x4_t ret;
#if defined(_SIMD_ARM_NEON)
  ret = vminq_f32(a, b);
#elif defined(_SIMD_X86_SSE)
  __m128 shuffle = _mm_shuffle_ps(a, a, _MM_SHUFFLE(2, 3, 0, 1));  // t2, t3, t0, t1
  a = _mm_max_ps(a, shuffle);                                      // max(t3, t2), max(t3, t2), max(t1, t0), max(t1, t0)
  shuffle = _mm_shuffle_ps(a, a, _MM_SHUFFLE(1, 0, 3, 2));         // max(t1, t0), max(t1, t0), max(t3, t2), max(t3, t2)
  ret = _mm_max_ps(a, shuffle);                                    // max(t3, t2, t1, t0), ...
#else
  ret.f32[0] = ret.f32[1] = ret.f32[2] = ret.f32[3] = fminf(fminf(fminf(a.f32[0], a.f32[1]), a.f32[2]), a.f32[3]);
#endif
  return ret;
}

// [vector] clamp(a, min, max)
static inline float32x4_t _SIMD_CALLCONV float32x4_clamp(float32x4_t a, float32x4_t min, float32x4_t max) {
  float32x4_t ret;
#if defined(_SIMD_ARM_NEON)
  ret = vminq_f32(vmaxq_f32(a, min), max);
#elif defined(_SIMD_X86_SSE)
  ret = _mm_min_ps(_mm_max_ps(a, min), max);
#else
  for (size_t i = 0; i < 4; ++i) {
    ret.f32[i] = fminf(fmaxf(a.f32[i], min.f32[i]), max.f32[i]);
  }
#endif
  return ret;
}

// [scalar] clamp(a, min, max)
static inline float32x4_t _SIMD_CALLCONV float32x4_clamps(float32x4_t a, float min, float max) {
  return float32x4_clamp(a, float32x4_inits(min), float32x4_inits(max));
}

// saturate(a)
static inline float32x4_t _SIMD_CALLCONV float32x4_saturate(float32x4_t a) {
  return float32x4_clamps(a, 0.F, 1.F);
}

#ifdef __cplusplus
}
#endif

#endif  // _SIMD_FLOAT32X4_H
