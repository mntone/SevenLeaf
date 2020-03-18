#ifndef _OSUTIL_UINT128_T_H
#define _OSUTIL_UINT128_T_H

#include "osutil_base.h"

#ifdef _MSC_VER
#include <intrin.h>
#endif
#include <immintrin.h>
#include <xmmintrin.h>

typedef union {
  struct {
    uint64_t low;
    uint64_t high;
  };
#ifdef _MSC_VER
  __m128i u128;
#else
  unsigned __int128 u128;
  __m128i __simd;
#endif
} osutil_uint128_t;

#ifdef __cplusplus
extern "C" {
#endif

static inline osutil_uint128_t osutil_uadd128(osutil_uint128_t x, osutil_uint128_t y) {
  osutil_uint128_t ret;
  uint8_t carry = _addcarry_u64(0, x.low, y.low, &ret.low);
  _addcarry_u64(carry, x.high, y.high, &ret.high);
  return ret;
}

static inline osutil_uint128_t osutil_usub128(osutil_uint128_t x, osutil_uint128_t y) {
  osutil_uint128_t ret;
  uint8_t borrow = _subborrow_u64(0, x.low, y.low, &ret.low);
  _subborrow_u64(borrow, x.high, y.high, &ret.high);
  return ret;
}

static inline osutil_uint128_t osutil_umul64x64(uint64_t multiplier, uint64_t multiplicand) {
  osutil_uint128_t ret;
#ifdef _MSC_VER
  ret.low = _umul128(multiplier, multiplicand, &ret.high);
#else
  ret.u128 = (unsigned __int128)multiplier * (unsigned __int128)multiplicand;
#endif
  return ret;
}

static inline uint64_t osutil_udiv128(osutil_uint128_t dividend, uint64_t divisor) {
#ifdef _MSC_VER
  uint64_t ret, remainder;
  ret = _udiv128(dividend.high, dividend.low, divisor, remainder);
#else
  uint64_t ret;
  ret = dividend.u128 / divisor;
#endif
  return ret;
}

#if 0
static inline uint64_t osutil_udiv128r(osutil_uint128_t dividend, uint64_t divisor, uint64_t *remainder) {
  uint64_t ret;
  ret = _udiv128(dividend.high, dividend.low, divisor, remainder);
  return ret;
}
#endif

static inline osutil_uint128_t osutil_uand128(osutil_uint128_t x, osutil_uint128_t y) {
  osutil_uint128_t ret;
  ret.__simd = _mm_and_si128(x.__simd, y.__simd);
  return ret;
}

static inline osutil_uint128_t osutil_uandnot128(osutil_uint128_t x, osutil_uint128_t y) {
  osutil_uint128_t ret;
  ret.__simd = _mm_andnot_si128(x.__simd, y.__simd);
  return ret;
}

static inline osutil_uint128_t osutil_uor128(osutil_uint128_t x, osutil_uint128_t y) {
  osutil_uint128_t ret;
  ret.__simd = _mm_or_si128(x.__simd, y.__simd);
  return ret;
}

static inline osutil_uint128_t osutil_uxor128(osutil_uint128_t x, osutil_uint128_t y) {
  osutil_uint128_t ret;
  ret.__simd = _mm_xor_si128(x.__simd, y.__simd);
  return ret;
}

#ifdef __cplusplus
}
#endif

#endif  // _OSUTIL_UINT128_T_H
