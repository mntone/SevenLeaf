#ifndef _CPSR_SIMD_FLOAT32X2_H
#define _CPSR_SIMD_FLOAT32X2_H

#include "../CpsrTypedefs.h" // TODO: remove header

#include <math.h>

#if defined(ARCH_INTEL)
// Intel (x86, x86-64)
// clang-format off
#ifdef DISABLE_SSE_EXTENDED
#  include <xmmintrin.h>
#else
#  include <nmmintrin.h>
#endif
// clang-format on
typedef __m64 simd_float32x2_t;

#define TO128(__M64__) _mm_castsi128_ps(_mm_cvtsi64_si128(*(long long *)&__M64__))
#define TO64(__M128__) (__m64)_mm_cvtsi128_si64(_mm_castps_si128(__M128__))

static inline simd_float32x2_t simd_float32x2_setzero() {
  return _mm_setzero_si64();
}

static inline simd_float32x2_t simd_float32x2_set(float x, float y) {
  return _mm_set_pi32(*(int *)&y, *(int *)&x);
}

static inline simd_float32x2_t simd_float32x2_setf(float f) {
  int as_int = *(int *)&f;
  return _mm_set_pi32(as_int, as_int);
}

static inline simd_float32x2_t simd_float32x2_neg(simd_float32x2_t a) {
  float zero = -0.0F;
  int zero_as_int = *(int *)&zero;
  return _mm_xor_si64(_mm_set1_pi32(zero_as_int), a);
}

static inline simd_float32x2_t simd_float32x2_abs(simd_float32x2_t a) {
  float zero = -0.0F;
  int zero_as_int = *(int *)&zero;
  return _mm_andnot_si64(_mm_set1_pi32(zero_as_int), a);
}

static inline simd_float32x2_t simd_float32x2_inv_lp(simd_float32x2_t a) {
  return TO64(_mm_rcp_ps(TO128(a)));
}

static inline simd_float32x2_t simd_float32x2_sqrt(simd_float32x2_t a) {
  return TO64(_mm_sqrt_ps(TO128(a)));
}

static inline simd_float32x2_t simd_float32x2_inv(simd_float32x2_t a) {
  // Newton Raphson
  __m128 a128 = TO128(a);
  __m128 reciprocal = _mm_rcp_ps(a128);
  __m128 muls = _mm_mul_ps(_mm_mul_ps(a128, reciprocal), reciprocal);
  return TO64(_mm_sub_ps(_mm_add_ps(reciprocal, reciprocal), muls));
}

static inline simd_float32x2_t simd_float32x2_sqrtinv_lp(simd_float32x2_t a) {
  return TO64(_mm_rsqrt_ps(TO128(a)));
}

static inline simd_float32x2_t simd_float32x2_sqrtinv(simd_float32x2_t a) {
  // Newton Raphson
  __m128 a128 = TO128(a);
  __m128 reciprocal = _mm_rsqrt_ps(a128);
  __m128 muls = _mm_mul_ps(_mm_mul_ps(a128, reciprocal), reciprocal);
  return TO64(_mm_mul_ps(_mm_sub_ps(_mm_set1_ps(3.F), muls), _mm_mul_ps(_mm_set1_ps(0.5F), reciprocal)));
}

static inline simd_float32x2_t simd_float32x2_add(simd_float32x2_t a, simd_float32x2_t b) {
  return TO64(_mm_add_ps(TO128(a), TO128(b)));
}

static inline simd_float32x2_t simd_float32x2_hadd(simd_float32x2_t a, simd_float32x2_t b) {
  return TO64(_mm_hadd_ps(TO128(a), TO128(b)));
}

static inline simd_float32x2_t simd_float32x2_sub(simd_float32x2_t a, simd_float32x2_t b) {
  return TO64(_mm_sub_ps(TO128(a), TO128(b)));
}

static inline simd_float32x2_t simd_float32x2_hsub(simd_float32x2_t a, simd_float32x2_t b) {
  return TO64(_mm_hsub_ps(TO128(a), TO128(b)));
}

static inline simd_float32x2_t simd_float32x2_addsub(simd_float32x2_t a, simd_float32x2_t b) {
#ifdef __PMMINTRIN_H
  return TO64(_mm_addsub_ps(TO128(a), TO128(b)));
#else
  simd_float32x2_t ret;
  float *aVal = &a;
  float *bVal = &b;
  float *cVal = &ret;
  cVal[0] = aVal[0] + bVal[0];
  cVal[1] = aVal[1] - bVal[1];
  return ret;
#endif
}

static inline simd_float32x2_t simd_float32x2_addsubf(simd_float32x2_t a, float f) {
#ifdef __PMMINTRIN_H
  return TO64(_mm_addsub_ps(TO128(a), _mm_set1_ps(f)));
#else
  simd_float32x2_t ret;
  float *aVal = &a;
  float *cVal = &ret;
  cVal[0] = aVal[0] + f;
  cVal[1] = aVal[1] - f;
  return ret;
#endif
}

static inline simd_float32x2_t simd_float32x2_subabs(simd_float32x2_t a, simd_float32x2_t b) {
  return simd_float32x2_abs(TO64(_mm_sub_ps(TO128(a), TO128(b))));
}

static inline simd_float32x2_t simd_float32x2_mul(simd_float32x2_t a, simd_float32x2_t b) {
  return TO64(_mm_mul_ps(TO128(a), TO128(b)));
}

static inline simd_float32x2_t simd_float32x2_mulf(simd_float32x2_t a, float f) {
  return TO64(_mm_mul_ps(TO128(a), _mm_set1_ps(f)));
}

static inline simd_float32x2_t simd_float32x2_muladd(simd_float32x2_t a, simd_float32x2_t b, simd_float32x2_t c) {
  return TO64(_mm_add_ps(TO128(a), _mm_mul_ps(TO128(b), TO128(c))));
}

static inline simd_float32x2_t simd_float32x2_mulsub(simd_float32x2_t a, simd_float32x2_t b, simd_float32x2_t c) {
  return TO64(_mm_sub_ps(TO128(a), _mm_mul_ps(TO128(b), TO128(c))));
}

static inline simd_float32x2_t simd_float32x2_div(simd_float32x2_t a, simd_float32x2_t b) {
  return TO64(_mm_div_ps(TO128(a), TO128(b)));
}

static inline simd_float32x2_t simd_float32x2_divf(simd_float32x2_t a, float f) {
  return TO64(_mm_div_ps(TO128(a), _mm_set1_ps(f)));
}

static inline float simd_float32x2_dot(simd_float32x2_t a, simd_float32x2_t b) {
#ifdef __NMMINTRIN_H
  return _mm_cvtss_f32(_mm_dp_ps(TO128(a), TO128(b), 0x31));
#else
  __m128 t = _mm_mul_ps(TO128(a), TO128(b));
#ifdef __PMMINTRIN_H
  // NOTE: https://stackoverflow.com/a/4121295
  __m128 shuffle = _mm_movehdup_ps(t);
#else
  __m128 shuffle = _mm_shuffle_ps(t, t, _MM_SHUFFLE(2, 3, 0, 1));;
#endif
  t = _mm_add_ps(t, shuffle);
  return _mm_cvtss_f32(t);
#endif
}

static inline simd_float32x2_t simd_float32x2_min(simd_float32x2_t a, simd_float32x2_t b) {
  return TO64(_mm_min_ps(TO128(a), TO128(b)));
}

static inline simd_float32x2_t simd_float32x2_max(simd_float32x2_t a, simd_float32x2_t b) {
  return TO64(_mm_max_ps(TO128(a), TO128(b)));
}

static inline simd_float32x2_t simd_float32x2_and(simd_float32x2_t a, simd_float32x2_t b) {
  return _mm_and_si64(a, b);
}

static inline simd_float32x2_t simd_float32x2_or(simd_float32x2_t a, simd_float32x2_t b) {
  return _mm_or_si64(a, b);
}

static inline simd_float32x2_t simd_float32x2_xor(simd_float32x2_t a, simd_float32x2_t b) {
  return _mm_xor_si64(a, b);
}

#undef TO128
#undef TO64

#elif defined(ARCH_ARM)
// ARM (ARMv7+)
#include <arm_neon.h>

typedef float32x2_t simd_float32x2_t;

static inline simd_float32x2_t simd_float32x2_setzero() {
  return vmov_n_f32(0.F);
}

static inline simd_float32x2_t simd_float32x2_set(float x, float y) {
  float v[2] = { y, x };
  return vld1_f32(v);
}

static inline simd_float32x2_t float32x4_setf(float f) {
  return vmov_n_f32(f);
}

static inline simd_float32x2_t simd_float32x2_neg(simd_float32x2_t a) {
  return vneg_f32(a);
}

static inline simd_float32x2_t simd_float32x2_abs(simd_float32x2_t a) {
  return vabs_f32(a);
}

static inline simd_float32x2_t simd_float32x2_inv_lp(simd_float32x2_t a) {
  return vrecps_f32(a);
}

static inline simd_float32x2_t simd_float32x2_inv(simd_float32x2_t a) {
  // Newton Raphson
  simd_float32x2_t reciprocal = vrsqrte_f32(a);
  reciprocal = vmul_f32(vrecps_f32(a, reciprocal), reciprocal);
  reciprocal = vmul_f32(vrecps_f32(a, reciprocal), reciprocal);
  return reciprocal;
}

static inline simd_float32x2_t simd_float32x2_sqrt(simd_float32x2_t a) {
  return vsqrt_f32(a);
}

static inline simd_float32x2_t simd_float32x2_sqrtinv_lp(simd_float32x2_t a) {
  return vrsqrte_f32(a);
}

static inline simd_float32x2_t simd_float32x2_sqrtinv(simd_float32x2_t a) {
  // Newton Raphson
  simd_float32x2_t reciprocal = vrsqrte_f32(a);
  reciprocal = vmul_f32(vrsqrts_f32(vmul_f32(reciprocal, reciprocal), a), reciprocal);
  reciprocal = vmul_f32(vrsqrts_f32(vmul_f32(reciprocal, reciprocal), a), reciprocal);
  return reciprocal;
}

static inline simd_float32x2_t simd_float32x2_add(simd_float32x2_t a, simd_float32x2_t b) {
  return vadd_f32(a, b);
}

static inline simd_float32x2_t simd_float32x2_hadd(simd_float32x2_t a, simd_float32x2_t b) {
  return vpadd_f32(a, b);
}

static inline simd_float32x2_t simd_float32x2_sub(simd_float32x2_t a, simd_float32x2_t b) {
  return vsub_f32(a, b);
}

static inline simd_float32x2_t simd_float32x2_hsub(simd_float32x2_t a, simd_float32x2_t b) {
  return vpadd_f32(a, vneg_f32(b));
}

static inline simd_float32x2_t simd_float32x2_addsub(simd_float32x2_t a, simd_float32x2_t b) {
  simd_float32x2_t ret;
  float *aVal = &a;
  float *bVal = &b;
  float *cVal = &ret;
  cVal[0] = aVal[0] + bVal[0];
  cVal[1] = aVal[1] - bVal[1];
  return ret;
}

static inline simd_float32x2_t simd_float32x2_addsub(simd_float32x2_t a, float f) {
  simd_float32x2_t ret;
  float *aVal = &a;
  float *cVal = &ret;
  cVal[0] = aVal[0] + f;
  cVal[1] = aVal[1] - f;
  return ret;
}

static inline simd_float32x2_t simd_float32x2_subabs(simd_float32x2_t a, simd_float32x2_t b) {
  return vabd_f32(a, b);
}

static inline simd_float32x2_t simd_float32x2_mul(simd_float32x2_t a, simd_float32x2_t b) {
  return vmul_f32(a, b);
}

static inline simd_float32x2_t simd_float32x2_mulf(simd_float32x2_t a, float f) {
  return vmul_n_f32(a, f);
}

static inline simd_float32x2_t simd_float32x2_muladd(simd_float32x2_t a, simd_float32x2_t b, simd_float32x2_t c) {
  return vmla_f32(a, b, c);
}

static inline simd_float32x2_t simd_float32x2_mulsub(simd_float32x2_t a, simd_float32x2_t b, simd_float32x2_t c) {
  return vmls_f32(a, b, c);
}

static inline simd_float32x2_t simd_float32x2_div(simd_float32x2_t a, simd_float32x2_t b) {
  return vdiv_f32(a, b);
}

static inline simd_float32x2_t simd_float32x2_divf(simd_float32x2_t a, simd_float32x2_t b) {
  return vdiv_f32(a, vmov_n_f32(b));
}

static inline float simd_float32x2_dot(simd_float32x2_t a, simd_float32x2_t b) {
  simd_float32x2_t prod = vmul_f32(a, b);
  return vaddv_f32(vadd_f32(prod, vadd_f32(prod, prod)));
}

static inline simd_float32x2_t simd_float32x2_min(simd_float32x2_t a, simd_float32x2_t b) {
  return vmin_f32(a, b);
}

static inline simd_float32x2_t simd_float32x2_max(simd_float32x2_t a, simd_float32x2_t b) {
  return vmax_f32(a, b);
}

static inline simd_float32x2_t simd_float32x2_and(simd_float32x2_t a, simd_float32x2_t b) {
  return vreinterpret_f32_u32(vand_u32(vreinterpret_u32_f32(a), vreinterpret_u32_f32(b)));
}

static inline simd_float32x2_t simd_float32x2_or(simd_float32x2_t a, simd_float32x2_t b) {
  return vreinterpret_f32_u32(vorr_u32(vreinterpret_u32_f32(a), vreinterpret_u32_f32(b)));
}

static inline simd_float32x2_t simd_float32x2_xor(simd_float32x2_t a, simd_float32x2_t b) {
  return vreinterpret_f32_u32(veor_u32(vreinterpret_u32_f32(a), vreinterpret_u32_f32(b)));
}

#else
#error "Unknown architecture."
#endif

// Utils
static inline simd_float32x2_t simd_float32x2_clamp(simd_float32x2_t a, simd_float32x2_t min, simd_float32x2_t max) {
  return simd_float32x2_max(simd_float32x2_min(a, min), max);
}

static inline simd_float32x2_t simd_float32x2_clampf(simd_float32x2_t a, float min, float max) {
  return simd_float32x2_max(simd_float32x2_min(a, simd_float32x2_setf(min)), simd_float32x2_setf(max));
}

static inline float simd_float32x2_length_squared(simd_float32x2_t a, simd_float32x2_t b) {
  return simd_float32x2_dot(a, b);
}

static inline float simd_float32x2_length(simd_float32x2_t a, simd_float32x2_t b) {
  return sqrtf(simd_float32x2_length_squared(a, b));
}

static inline float simd_float32x2_distance_squared(simd_float32x2_t a, simd_float32x2_t b) {
  simd_float32x2_t d = simd_float32x2_sub(a, b);
  return simd_float32x2_dot(d, d);
}

static inline float simd_float32x2_distance(simd_float32x2_t a, simd_float32x2_t b) {
  return sqrtf(simd_float32x2_distance_squared(a, b));
}

#endif // _CPSR_SIMD_FLOAT32X2_H
