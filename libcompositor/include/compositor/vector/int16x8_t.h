#ifndef _SIMD_INT16X8_H
#define _SIMD_INT16X8_H

#include "simd_base.h"

#ifdef __cplusplus
extern "C" {
#endif

#define _SIMD_CF(__element) (((2 * __element + 1) << 8) | (2 * __element))

#ifdef _SIMD_X86

static inline __m128i _simd_mm_neg_epi16(__m128i __a) {
  return _mm_sign_epi16(__a, _mm_set1_epi16(-1));
}

#endif

// ---
// Constants
// ---
// clang-format off
#define INT16X8_ZERO int16x8_inits(0)
#define INT16X8_ONE  int16x8_inits(1)
#define INT16X8_MIN  int16x8_inits(INT16_MIN)
#define INT16X8_MAX  int16x8_inits(INT16_MAX)
// clang-format on

// ---
// Inits
// ---
static inline int16x8_t
int16x8_t_initv(int16_t s0, int16_t s1, int16_t s2, int16_t s3, int16_t s4, int16_t s5, int16_t s6, int16_t s7) {
  uint16x8_t ret;
#if defined(_SIMD_ARM_NEON)
  ret = { s0, s1, s2, s3, s4, s5, s6, s7 };
#elif defined(_SIMD_X86_SSE2)
  ret = _mm_setr_epi16(s0, s1, s2, s3, s4, s5, s6, s7);
#else
  ret.s16[0] = s0;
  ret.s16[1] = s1;
  ret.s16[2] = s2;
  ret.s16[3] = s3;
  ret.s16[4] = s4;
  ret.s16[5] = s5;
  ret.s16[6] = s6;
  ret.s16[7] = s7;
#endif
  return ret;
}

static inline int16x8_t int16x8_inits(int16_t s) {
  int16x8_t ret;
#if defined(_SIMD_ARM_NEON)
  ret = vdupq_n_s16(s);
#elif defined(_SIMD_X86_SSE2)
  ret = _mm_set1_epi16(s);
#else
  for (size_t i = 0; i < 8; ++i) {
    ret.s16[i] = s;
  }
#endif
  return ret;
}

static inline int16x8_t int16x8_inita(const int16_t a[]) {
  int16x8_t ret;
#if defined(_SIMD_ARM_NEON)
  ret = vld1q_s16(a);
#elif defined(_SIMD_X86_SSE2)
  ret = _mm_load_si128((const __m128i *)a);
#else
  for (size_t i = 0; i < 8; ++i) {
    ret.s16[i] = a[i];
  }
#endif
  return ret;
}

static inline int16x8_t int16x8_initp(const int16_t *p) {
  int16x8_t ret;
#if defined(_SIMD_ARM_NEON)
  ret = vld1q_dup_s16(p);
#elif defined(_SIMD_X86_SSE2)
  ret = _mm_set1_epi16(*p);
#else
  int16_t s = *p;
  for (size_t i = 0; i < 8; ++i) {
    ret.s16[i] = s;
  }
#endif
  return ret;
}

// ---
// Gets
// ---
static inline int16_t int16x8_getat(int16x8_t v, int index) {
  int16_t ret;
#if defined(_SIMD_ARM_NEON)
  ret = vst1q_lane_u16(p, v, index);
#elif defined(_SIMD_X86_SSE2)
  switch (index) {
  case 0: ret = (int16_t)_mm_extract_epi16(v, 0); break;
  case 1: ret = (int16_t)_mm_extract_epi16(v, 1); break;
  case 2: ret = (int16_t)_mm_extract_epi16(v, 2); break;
  case 3: ret = (int16_t)_mm_extract_epi16(v, 3); break;
  case 4: ret = (int16_t)_mm_extract_epi16(v, 4); break;
  case 5: ret = (int16_t)_mm_extract_epi16(v, 5); break;
  case 6: ret = (int16_t)_mm_extract_epi16(v, 6); break;
  case 7: ret = (int16_t)_mm_extract_epi16(v, 7); break;
  default: unreachable();
  }
#else
  ret = v.s16[index];
#endif
  return ret;
}

static inline void int16x8_getatp(int16x8_t v, int index, int16_t *p) {
#if defined(_SIMD_ARM_NEON)
  ret = vst1q_lane_s16(value, v, index);
#elif defined(_SIMD_X86_SSE2)
  switch (index) {
  case 0: *p = (int16_t)_mm_extract_epi16(v, 0); break;
  case 1: *p = (int16_t)_mm_extract_epi16(v, 1); break;
  case 2: *p = (int16_t)_mm_extract_epi16(v, 2); break;
  case 3: *p = (int16_t)_mm_extract_epi16(v, 3); break;
  case 4: *p = (int16_t)_mm_extract_epi16(v, 4); break;
  case 5: *p = (int16_t)_mm_extract_epi16(v, 5); break;
  case 6: *p = (int16_t)_mm_extract_epi16(v, 6); break;
  case 7: *p = (int16_t)_mm_extract_epi16(v, 7); break;
  default: unreachable();
  }
#else
  *p = v.s16[index];
#endif
}

// ---
// Sets
// ---
static inline int16x8_t int16x8_setat(int16x8_t v, int index, int16_t value) {
  int16x8_t ret;
#if defined(_SIMD_ARM_NEON)
  ret = vsetq_lane_u16(value, v, index);
#elif defined(_SIMD_X86_SSE2)
  switch (index) {
  case 0: ret = _mm_insert_epi16(v, value, 0); break;
  case 1: ret = _mm_insert_epi16(v, value, 1); break;
  case 2: ret = _mm_insert_epi16(v, value, 2); break;
  case 3: ret = _mm_insert_epi16(v, value, 3); break;
  case 4: ret = _mm_insert_epi16(v, value, 4); break;
  case 5: ret = _mm_insert_epi16(v, value, 5); break;
  case 6: ret = _mm_insert_epi16(v, value, 6); break;
  case 7: ret = _mm_insert_epi16(v, value, 7); break;
  default: unreachable();
  }
#else
  ret.s16[index] = value;
#endif
  return ret;
}

// ---
// Packing
// ---
// a0, b0, a1, b1, a2, b2, a3, b3
static inline int16x8_t int16x8_unpack16_lo(int16x8_t a, int16x8_t b) {
  uint16x8_t ret;
#if defined(_SIMD_ARM_NEON)
  ret = vzipq_s16(a, b).val[0];
#elif defined(_SIMD_X86_SSE2)
  ret = _mm_unpacklo_epi16(a, b);
#else
  for (size_t i = 0; i < 4; ++i) {
    ret.s16[2 * i] = a.s16[i];
    ret.s16[2 * i + 1] = b.s16[i];
  }
#endif
  return ret;
}

// a4, b4, a5, b5, a6, b6, a7, b7
static inline int16x8_t int16x8_unpack16_hi(int16x8_t a, int16x8_t b) {
  int16x8_t ret;
#if defined(_SIMD_ARM_NEON)
  ret = vzipq_s16(a, b).val[1];
#elif defined(_SIMD_X86_SSE2)
  ret = _mm_unpackhi_epi16(a, b);
#else
  for (size_t i = 0; i < 4; ++i) {
    ret.s16[2 * i] = a.s16[4 + i];
    ret.s16[2 * i + 1] = b.s16[4 + i];
  }
#endif
  return ret;
}

// a0, b0, a1, b1, a2, b2, a3, b3 | a4, b4, a5, b5, a6, b6, a7, b7
static inline int16x8x2_t int16x8_unpack16(int16x8_t a, int16x8_t b) {
  int16x8x2_t ret;
#if defined(_SIMD_ARM_NEON)
  ret = vzipq_s16(a.v, b.v);
#elif defined(_SIMD_X86_SSE2)
  ret = int16x8x2_init(_mm_unpacklo_epi16(a, b), _mm_unpackhi_epi16(a, b));
#else
  for (size_t i = 0; i < 4; ++i) {
    ret.val[0].s16[2 * i] = a.s16[i];
    ret.val[0].s16[2 * i + 1] = b.s16[i];
  }
  for (size_t i = 0; i < 4; ++i) {
    ret.val[1].s16[2 * i] = a.s16[4 + i];
    ret.val[1].s16[2 * i + 1] = b.s16[4 + i];
  }
#endif
  return ret;
}

// a0, a1, b0, b1, a2, a3, b2, b3
static inline int16x8_t int16x8_unpack32_lo(int16x8_t a, int16x8_t b) {
  int16x8_t ret;
#if defined(_SIMD_ARM_NEON)
  ret = ((int16x8x2_t)vzipq_s32((int32x4_t)a, (int32x4_t)b)).val[0];
#elif defined(_SIMD_X86_SSE2)
  ret = _mm_unpacklo_epi32(a, b);
#else
  for (size_t i = 0; i < 2; ++i) {
    ret.s32[2 * i] = a.s32[i];
    ret.s32[2 * i + 1] = b.s32[i];
  }
#endif
  return ret;
}

// a4, a5, b4, b5, a6, a7, b6, b7
static inline int16x8_t int16x8_unpack32_hi(int16x8_t a, int16x8_t b) {
  int16x8_t ret;
#if defined(_SIMD_ARM_NEON)
  ret = ((int16x8x2_t)vzipq_s32((int32x4_t)a, (int32x4_t)b)).val[1];
#elif defined(_SIMD_X86_SSE2)
  ret = _mm_unpackhi_epi32(a, b);
#else
  for (size_t i = 0; i < 2; ++i) {
    ret.s32[2 * i] = a.s32[2 + i];
    ret.s32[2 * i + 1] = b.s32[2 + i];
  }
#endif
  return ret;
}

// a0, a1, b0, b1, a2, a3, b2, b3 | a4, a5, b4, b5, a6, a7, b6, b7
static inline int16x8x2_t int16x8_unpack32(int16x8_t a, int16x8_t b) {
  int16x8x2_t ret;
#if defined(_SIMD_ARM_NEON)
  ret = (int16x8x2_t)vzipq_u32((int32x4_t)a, (int32x4_t)b);
#elif defined(_SIMD_X86_SSE2)
  ret = int16x8x2_init(_mm_unpacklo_epi32(a, b), _mm_unpackhi_epi32(a, b));
#else
  for (size_t i = 0; i < 2; ++i) {
    ret.val[0].s32[2 * i] = a.s32[i];
    ret.val[0].s32[2 * i + 1] = b.s32[i];
  }
  for (size_t i = 0; i < 2; ++i) {
    ret.val[1].s32[2 * i] = a.s32[2 + i];
    ret.val[1].s32[2 * i + 1] = b.s32[2 + i];
  }
#endif
  return ret;
}

// a0, a1, a2, a3, b0, b1, b2, b3
static inline int16x8_t int16x8_unpack64_lo(int16x8_t a, int16x8_t b) {
  int16x8_t ret;
#if defined(_SIMD_ARM_NEON)
  ret = vcombine_s16(vget_low_s16(a), vget_low_s16(b));
#elif defined(_SIMD_X86_SSE2)
  ret = _mm_unpacklo_epi64(a, b);
#else
  ret.s64[0] = a.s64[0];
  ret.s64[1] = b.s64[0];
#endif
  return ret;
}

// a4, a5, a6, a7, b4, b5, b6, b7
static inline int16x8_t int16x8_unpack64_hi(int16x8_t a, int16x8_t b) {
  int16x8_t ret;
#if defined(_SIMD_ARM_NEON)
  ret = vcombine_s16(vget_high_s16(a), vget_high_s16(b));
#elif defined(_SIMD_X86_SSE2)
  ret = _mm_unpackhi_epi64(a, b);
#else
  ret.s64[0] = a.s64[1];
  ret.s64[1] = b.s64[1];
#endif
  return ret;
}

// a0, a1, b0, b1, a2, a3, b2, b3 | a4, a5, a6, a7, b4, b5, b6, b7
static inline int16x8x2_t int16x8_unpack64(int16x8_t a, int16x8_t b) {
  int16x8x2_t ret;
#if defined(_SIMD_ARM_NEON)
  ret.val[0] = vcombine_s16(vget_low_s16(a), vget_low_s16(b));
  ret.val[1] = vcombine_s16(vget_high_s16(a), vget_high_s16(b));
#elif defined(_SIMD_X86_SSE2)
  ret = int16x8x2_init(_mm_unpacklo_epi64(a, b), _mm_unpackhi_epi64(a, b));
#else
  ret.val[0].s64[0] = a.s64[0];
  ret.val[0].s64[1] = b.s64[0];
  ret.val[1].s64[0] = a.s64[1];
  ret.val[1].s64[1] = b.s64[1];
#endif
  return ret;
}

// a1, a0, a3, a2, a5, a4, a7, a6
static inline int16x8_t int16x8_reverse32p16(int16x8_t a) {
  int16x8_t ret;
#if defined(_SIMD_ARM_NEON)
  ret = vrev32q_s16(a);
#elif defined(_SIMD_X86_SSSE3)
  __m128i mask = _mm_setr_epi8(0x2, 0x3, 0x0, 0x1, 0x6, 0x7, 0x4, 0x5, 0xA, 0xB, 0x8, 0x9, 0xE, 0xF, 0xC, 0xD);
  ret = _mm_shuffle_epi8(a, mask);
#elif defined(_SIMD_X86_SSE2)
  ret = _mm_shufflehi_epi16(_mm_shufflelo_epi16(a, _MM_SHUFFLE(2, 3, 0, 1)), _MM_SHUFFLE(2, 3, 0, 1));
#else
  for (size_t i = 0; i < 8; i += 2) {
    ret.s16[i] = a.s16[i + 1];
    ret.s16[i + 1] = a.s16[i];
  }
#endif
  return ret;
}

// a3, a2, a1, a0, a7, a6, a5, a4
static inline int16x8_t int16x8_reverse64p16(int16x8_t a) {
  int16x8_t ret;
#if defined(_SIMD_ARM_NEON)
  ret = vrev64q_s16(a);
#elif defined(_SIMD_X86_SSSE3)
  __m128i mask = _mm_setr_epi8(0x6, 0x7, 0x4, 0x5, 0x2, 0x3, 0x0, 0x1, 0xE, 0xF, 0xC, 0xD, 0xA, 0xB, 0x8, 0x9);
  ret = _mm_shuffle_epi8(a, mask);
#elif defined(_SIMD_X86_SSE2)
  ret = _mm_shufflehi_epi16(_mm_shufflelo_epi16(a, _MM_SHUFFLE(3, 2, 1, 0)), _MM_SHUFFLE(3, 2, 1, 0));
#else
  for (size_t i = 0; i < 8; i += 4) {
    ret.s16[i] = a.s16[i + 3];
    ret.s16[i + 1] = a.s16[i + 2];
    ret.s16[i + 2] = a.s16[i + 1];
    ret.s16[i + 3] = a.s16[i];
  }
#endif
  return ret;
}

// a7, a6, a5, a4, a3, a2, a1, a0
static inline int16x8_t int16x8_reverse128p16(int16x8_t a) {
  int16x8_t ret;
#if defined(_SIMD_ARM_NEON)
  int16x8_t ar = int16x8_reverse64p16(a);
  ret = vcombine_s16(vget_high_s16(ar), vget_low_s16(ar));
#elif defined(_SIMD_X86_SSSE3)
  __m128i mask = _mm_setr_epi8(0xE, 0xF, 0xC, 0xD, 0xA, 0xB, 0x8, 0x9, 0x6, 0x7, 0x4, 0x5, 0x2, 0x3, 0x0, 0x1);
  ret = _mm_shuffle_epi8(a, mask);
#elif defined(_SIMD_X86_SSE2)
  ret = _mm_shuffle_epi32(int16x8_reverse64p16(a), _MM_SHUFFLE(1, 0, 3, 2));
#else
  for (size_t i = 0; i < 8; ++i) {
    ret.s16[i] = a.s16[7 - i];
  }
#endif
  return ret;
}

// a2, a3, a0, a1, a6, a7, a4, a5
static inline int16x8_t int16x8_reverse64p32(int16x8_t a) {
  int16x8_t ret;
#if defined(_SIMD_ARM_NEON)
  ret = (int16x8_t)vrev64q_s32((uint32x4_t)a);
#elif defined(_SIMD_X86_SSSE3)
  __m128i mask = _mm_setr_epi8(0x4, 0x5, 0x6, 0x7, 0x0, 0x1, 0x2, 0x3, 0xC, 0xD, 0xE, 0xF, 0x8, 0x9, 0xA, 0xB);
  ret = _mm_shuffle_epi8(a, mask);
#elif defined(_SIMD_X86_SSE2)
  ret = _mm_shufflehi_epi16(_mm_shufflelo_epi16(a, _MM_SHUFFLE(1, 0, 3, 2)), _MM_SHUFFLE(1, 0, 3, 2));
#else
  for (size_t i = 0; i < 8; i += 4) {
    ret.s16[i] = a.s16[i + 2];
    ret.s16[i + 1] = a.s16[i + 3];
    ret.s16[i + 2] = a.s16[i];
    ret.s16[i + 3] = a.s16[i + 1];
  }
#endif
  return ret;
}

// a6, a7, a4, a5, a2, a3, a0, a1
static inline int16x8_t int16x8_reverse128p32(int16x8_t a) {
  int16x8_t ret;
#if defined(_SIMD_ARM_NEON)
  int16x8_t ar = int16x8_reverse64p32(a);
  ret = vcombine_s16(vget_high_s16(ar), vget_low_s16(ar));
#elif defined(_SIMD_X86_SSSE3)
  __m128i mask = _mm_setr_epi8(0xC, 0xD, 0xE, 0xF, 0x8, 0x9, 0xA, 0xB, 0x4, 0x5, 0x6, 0x7, 0x0, 0x1, 0x2, 0x3);
  ret = _mm_shuffle_epi8(a, mask);
#elif defined(_SIMD_X86_SSE2)
#elif defined(_SIMD_X86_SSE2)
  ret = _mm_shuffle_epi32(int16x8_reverse64p32(a), _MM_SHUFFLE(1, 0, 3, 2));
#else
  for (size_t i = 0; i < 4; ++i) {
    ret.s32[i] = a.s32[3 - i];
  }
#endif
  return ret;
}

// a4, a5, a6, a7, a0, a1, a2, a3
static inline int16x8_t int16x8_reverse128p64(int16x8_t a) {
  int16x8_t ret;
#if defined(_SIMD_ARM_NEON)
  ret = vcombine_s16(vget_high_s16(a), vget_low_s16(a));
#elif defined(_SIMD_X86_SSSE3)
  __m128i mask = _mm_setr_epi8(0x8, 0x9, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF, 0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7);
  ret = _mm_shuffle_epi8(a, mask);
#elif defined(_SIMD_X86_SSE2)
  ret = _mm_shuffle_epi32(a, _MM_SHUFFLE(1, 0, 3, 2));
#else
  ret.s64[0] = a.s64[1];
  ret.s64[1] = a.s64[0];
#endif
  return ret;
}

// c0, c1, c2, c3, c4, c5, c6, c7
#ifdef _SIMD_X86_SSE4_1
#define int16x8_blend(__a, __b, __mask) _mm_blend_epi16(__a, __b, __mask);
#else
static inline int16x8_t int16x8_blend(int16x8_t a, int16x8_t b, int mask) {
  int16x8_t ret;
#if defined(_SIMD_ARM_NEON)
  int16x8_t m = { mask & 0x01 ? 0xFFFF : 0, mask & 0x02 ? 0xFFFF : 0, mask & 0x04 ? 0xFFFF : 0,
                  mask & 0x08 ? 0xFFFF : 0, mask & 0x10 ? 0xFFFF : 0, mask & 0x20 ? 0xFFFF : 0,
                  mask & 0x40 ? 0xFFFF : 0, mask & 0x80 ? 0xFFFF : 0 };
  ret = vbslq_s16(m, a, b);
#elif defined(_SIMD_X86_SSE2)
  __m128i m = _mm_setr_epi16(mask & 0x01 ? 0xFFFF : 0,
                             mask & 0x02 ? 0xFFFF : 0,
                             mask & 0x04 ? 0xFFFF : 0,
                             mask & 0x08 ? 0xFFFF : 0,
                             mask & 0x10 ? 0xFFFF : 0,
                             mask & 0x20 ? 0xFFFF : 0,
                             mask & 0x40 ? 0xFFFF : 0,
                             mask & 0x80 ? 0xFFFF : 0);
  ret = _simd_mm_sel_si128(a, b, m);
#else
  for (size_t i = 0; i < 4; ++i) {
    ret.s16[2 * i] = a.s16[i];
    ret.s16[2 * i + 1] = b.s16[i];
  }
#endif
  return ret;
}
#endif

// a5, a6, a7, b0, b1, b2, b3, b4
#if defined(_SIMD_ARM_NEON)
#define int16x8_alignr(__a, __b, __align) vextq_s16(__b, __a, __align);
#elif defined(_SIMD_X86_SSE2)
#define int16x8_alignr(__a, __b, __align) _mm_alignr_epi8(__a, __b, 2 * __align);
#else
static inline int16x8_t int16x8_alignr(int16x8_t a, int16x8_t b, int align) {
  int16x8_t ret;
  for (size_t i = 0; i < align; ++i) {
    ret.s16[i] = a.s16[8 - i];
  }
  for (size_t i = align; i < 8; ++i) {
    ret.s16[1] = b.s16[i - align];
  }
  return ret;
}
#endif

// swizzle
static inline int16x8_t
int16x8_swizzle(int16x8_t a, size_t e0, size_t e1, size_t e2, size_t e3, size_t e4, size_t e5, size_t e6, size_t e7) {
  int16x8_t ret;
#if defined(_SIMD_ARM_NEON)
  int8x8x2_t ref;
  ref.val[0] = (int8x8_t)vget_low_s16(a);
  ref.val[1] = (int8x8_t)vget_high_s16(a);

  int8x8_t retlo =
      vtbl2_s8(ref, vcreate_s8(_SIMD_CF(e0) | (_SIMD_CF(e1) << 16) | (_SIMD_CF(e2) << 32) | (_SIMD_CF(e3) << 48)));
  int8x8_t rethi =
      vtbl2_s8(ref, vcreate_s8(_SIMD_CF(e4) | (_SIMD_CF(e5) << 16) | (_SIMD_CF(e6) << 32) | (_SIMD_CF(e7) << 48)));
  ret = vcombine_s16((int16x4_t)retlo, (int16x4_t)rethi);
#elif defined(_SIMD_X86_SSSE3)
  __m128i m = _mm_set_epi64x(_SIMD_CF(e4) | (_SIMD_CF(e5) << 16) | (_SIMD_CF(e6) << 32) | (_SIMD_CF(e7) << 48),
                             _SIMD_CF(e0) | (_SIMD_CF(e1) << 16) | (_SIMD_CF(e2) << 32) | (_SIMD_CF(e3) << 48));
  ret = _mm_shuffle_epi8(a, m);
#elif defined(_SIMD_X86_SSE2)
  const __simd128 *ap = (__simd128 *)&a;
  __simd128 *rp = (__simd128 *)&ret;
  rp->s16[0] = ap->s16[e0];
  rp->s16[1] = ap->s16[e1];
  rp->s16[2] = ap->s16[e2];
  rp->s16[3] = ap->s16[e3];
  rp->s16[4] = ap->s16[e4];
  rp->s16[5] = ap->s16[e5];
  rp->s16[6] = ap->s16[e6];
  rp->s16[7] = ap->s16[e7];
#else
  const __simd128 *ap = (__simd128 *)&a;
  ret.s16[0] = ap->s16[e0];
  ret.s16[1] = ap->s16[e1];
  ret.s16[2] = ap->s16[e2];
  ret.s16[3] = ap->s16[e3];
  ret.s16[4] = ap->s16[e4];
  ret.s16[5] = ap->s16[e5];
  ret.s16[6] = ap->s16[e6];
  ret.s16[7] = ap->s16[e7];
#endif
  return ret;
}

// ---
// Logic
// ---
// ~a
static inline int16x8_t int16x8_not(int16x8_t a) {
  int16x8_t ret;
#if defined(_SIMD_ARM_NEON)
  ret = vmvnq_s16(a);
#elif defined(_SIMD_X86_SSE2)
  ret = _simd_mm_not_si128(a);
#else
  ret = __simd128_not(a);
#endif
  return ret;
}

// a & b
static inline int16x8_t int16x8_and(int16x8_t a, int16x8_t b) {
  int16x8_t ret;
#if defined(_SIMD_ARM_NEON)
  ret = vandq_u16(a, b);
#elif defined(_SIMD_X86_SSE2)
  ret = _mm_and_si128(a, b);
#else
  ret = __simd128_and(a, b);
#endif
  return ret;
}

// ~a & b
static inline int16x8_t int16x8_andnot(int16x8_t a, int16x8_t b) {
  int16x8_t ret;
#if defined(_SIMD_ARM_NEON)
  ret = vbicq_s16(b, a);
#elif defined(_SIMD_X86_SSE2)
  ret = _mm_andnot_si128(a, b);
#else
  ret = __simd128_andnot(a, b);
#endif
  return ret;
}

// a | b
static inline int16x8_t int16x8_or(int16x8_t a, int16x8_t b) {
  int16x8_t ret;
#if defined(_SIMD_ARM_NEON)
  ret = vorrq_s16(a, b);
#elif defined(_SIMD_X86_SSE2)
  ret = _mm_or_si128(a, b);
#else
  ret = __simd128_or(a, b);
#endif
  return ret;
}

// ~a | b
static inline int16x8_t int16x8_ornot(int16x8_t a, int16x8_t b) {
  int16x8_t ret;
#if defined(_SIMD_ARM_NEON)
  ret = vornq_s16(b, a);
#elif defined(_SIMD_X86_SSE2)
  ret = _mm_or_si128(_simd_mm_not_si128(a), b);
#else
  ret = __simd128_ornot(a, b);
#endif
  return ret;
}

// a ^ b
static inline int16x8_t int16x8_xor(int16x8_t a, int16x8_t b) {
  int16x8_t ret;
#if defined(_SIMD_ARM_NEON)
  ret = veorq_s16(a, b);
#elif defined(_SIMD_X86_SSE2)
  ret = _mm_xor_si128(a, b);
#else
  ret = __simd128_xor(a, b);
#endif
  return ret;
}

// (a & ~m) | (b & m)
static inline int16x8_t int16x8_sel(int16x8_t a, int16x8_t b, int16x8_t m) {
  int16x8_t ret;
#if defined(_SIMD_ARM_NEON)
  ret = vbslq_s16(m, a, b);
#elif defined(_SIMD_X86_SSE2)
  ret = _simd_mm_sel_si128(a, b, m);
#else
  ret = __simd128_sel(a, b, m);
#endif
  return ret;
}

// a >> n
static inline int16x8_t int16x8_shr(int16x8_t a, int n) {
  uint16x8_t ret;
#if defined(_SIMD_ARM_NEON)
  ret = vshrq_n_u16(a, n);
#elif defined(_SIMD_X86_SSE2)
  ret = _mm_slli_epi16(a, n);
#else
  for (size_t i = 0; i < 8; ++i) {
    ret.u16[i] = a.u16[i] >> n;
  }
#endif
  return ret;
}

// a << n
static inline int16x8_t int16x8_shl(int16x8_t a, int n) {
  int16x8_t ret;
#if defined(_SIMD_ARM_NEON)
  ret = vshlq_n_s16(a, n);
#elif defined(_SIMD_X86_SSE2)
  ret = _mm_slli_epi16(a, n);
#else
  for (size_t i = 0; i < 8; ++i) {
    ret.u16[i] = a.u16[i] << n;
  }
#endif
  return ret;
}

// a >> n (arithmetic)
static inline int16x8_t int16x8_shra(int16x8_t a, int n) {
  uint16x8_t ret;
#if defined(_SIMD_ARM_NEON)
  ret = vshrq_n_s16(a, n);
#elif defined(_SIMD_X86_SSE2)
  ret = _mm_srai_epi16(a, n);
#else
  for (size_t i = 0; i < 8; ++i) {
    ret.u16[i] = a.s16[i] >> n;
  }
#endif
  return ret;
}

// ---
// Arithmetic
// ---
// -a
static inline int16x8_t int16x8_neg(int16x8_t a) {
  int16x8_t ret;
#if defined(_SIMD_ARM_NEON)
  ret = vnegq_s16(a);
#elif defined(_SIMD_X86_SSE)
  ret = _simd_mm_neg_epi16(a);
#else
  for (size_t i = 0; i < 8; ++i) {
    ret.s16[i] = -a.s16[i];
  }
#endif
  return ret;
}

// a + b
static inline int16x8_t int16x8_add(int16x8_t a, int16x8_t b) {
  uint16x8_t ret;
#if defined(_SIMD_ARM_NEON)
  ret = vaddq_s16(a, b);
#elif defined(_SIMD_X86_SSE2)
  ret = _mm_add_epi16(a, b);
#else
  for (size_t i = 0; i < 8; ++i) {
    ret.s16[i] = a.s16[i] + b.s16[i];
  }
#endif
  return ret;
}

// saturate(a + b)
static inline int16x8_t int16x8_adds(int16x8_t a, int16x8_t b) {
  uint16x8_t ret;
#if defined(_SIMD_ARM_NEON)
  ret = vqaddq_s16(a, b);
#elif defined(_SIMD_X86_SSE2)
  ret = _mm_adds_epi16(a, b);
#else
  for (size_t i = 0; i < 8; ++i) {
    int16_t ai = a.s16[i];
    int16_t bi = b.s16[i];
    ret.s16[i] = (ai > INT16_MAX - bi) ? INT16_MAX : ai + bi;
  }
#endif
  return ret;
}

// a - b
static inline int16x8_t int16x8_sub(int16x8_t a, int16x8_t b) {
  uint16x8_t ret;
#if defined(_SIMD_ARM_NEON)
  ret = vsubq_s16(a, b);
#elif defined(_SIMD_X86_SSE2)
  ret = _mm_sub_epi16(a, b);
#else
  for (size_t i = 0; i < 8; ++i) {
    ret.s16[i] = a.s16[i] - b.s16[i];
  }
#endif
  return ret;
}

// saturate(a - b)
static inline int16x8_t int16x8_subs(int16x8_t a, int16x8_t b) {
  uint16x8_t ret;
#if defined(_SIMD_ARM_NEON)
  ret = vqsubq_s16(a, b);
#elif defined(_SIMD_X86_SSE2)
  ret = _mm_subs_epi16(a, b);
#else
  for (size_t i = 0; i < 8; ++i) {
    int16_t ai = a.s16[i];
    int16_t bi = b.s16[i];
    ret.s16[i] = ai < bi ? INT16_MIN : ai - bi;
  }
#endif
  return ret;
}

// a * b
static inline int16x8_t int16x8_mul(int16x8_t a, int16x8_t b) {
  uint16x8_t ret;
#if defined(_SIMD_ARM_NEON)
  ret = vmulq_s16(a, b);
#elif defined(_SIMD_X86_SSE2)
  ret = _mm_mullo_epi16(a, b);
#else
  for (size_t i = 0; i < 8; ++i) {
    ret.s16[i] = a.s16[i] * b.s16[i];
  }
#endif
  return ret;
}

// ---
// Compare
// ---
// a == b
static inline int16x8_t int16x8_cmpeq(int16x8_t a, int16x8_t b) {
  int16x8_t ret;
#if defined(_SIMD_ARM_NEON)
  ret = vceq_s16(a, b);
#elif defined(_SIMD_X86_SSE2)
  ret = _mm_cmpeq_epi16(a, b);
#else
  for (size_t i = 0; i < 8; ++i) {
    ret.s16[i] = a.s16[i] == b.s16[i] ? 0xFFFF : 0;
  }
#endif
  return ret;
}

// a != b
static inline int16x8_t int16x8_cmpne(int16x8_t a, int16x8_t b) {
  int16x8_t ret;
#if defined(_SIMD_ARM_NEON)
  ret = vmvnq_s16(vceq_s16(a, b));
#elif defined(_SIMD_X86_SSE2)
  ret = _simd_mm_not_si128(_mm_cmpeq_epi16(a, b));
#else
  for (size_t i = 0; i < 8; ++i) {
    ret.s16[i] = a.s16[i] != b.s16[i] ? 0xFFFF : 0;
  }
#endif
  return ret;
}

// a > b
static inline int16x8_t int16x8_cmpgt(int16x8_t a, int16x8_t b) {
  int16x8_t ret;
#if defined(_SIMD_ARM_NEON)
  ret = vcgtq_s16(a, b);
#elif defined(_SIMD_X86_SSE2)
  ret = _mm_cmpgt_epi16(a, b);
#else
  for (size_t i = 0; i < 8; ++i) {
    ret.s16[i] = a.s16[i] > b.s16[i] ? 0xFFFF : 0;
  }
#endif
  return ret;
}

// a >= b
static inline int16x8_t int16x8_cmpge(int16x8_t a, int16x8_t b) {
  int16x8_t ret;
#if defined(_SIMD_ARM_NEON)
  ret = vcgeq_s16(a, b);
#elif defined(_SIMD_X86_SSE2)
  ret = _simd_mm_not_si128(_mm_cmpgt_epi16(b, a));
#else
  for (size_t i = 0; i < 8; ++i) {
    ret.s16[i] = a.s16[i] >= b.s16[i] ? 0xFFFF : 0;
  }
#endif
  return ret;
}

// a < b
static inline int16x8_t int16x8_cmplt(int16x8_t a, int16x8_t b) {
  int16x8_t ret;
#if defined(_SIMD_ARM_NEON)
  ret = vcltq_s16(a, b);
#elif defined(_SIMD_X86_SSE2)
  ret = _mm_cmpgt_epi16(b, a);
#else
  for (size_t i = 0; i < 8; ++i) {
    ret.s16[i] = a.s16[i] < b.s16[i] ? 0xFFFF : 0;
  }
#endif
  return ret;
}

// a <= b
static inline int16x8_t int16x8_cmple(int16x8_t a, int16x8_t b) {
  int16x8_t ret;
#if defined(_SIMD_ARM_NEON)
  ret = vcleq_s16(a, b);
#elif defined(_SIMD_X86_SSE2)
  ret = _simd_mm_not_si128(_mm_cmpgt_epi16(a, b));
#else
  for (size_t i = 0; i < 8; ++i) {
    ret.s16[i] = a.s16[i] <= b.s16[i] ? 0xFFFF : 0;
  }
#endif
  return ret;
}

// [vector] min(a, b)
static inline int16x8_t int16x8_min(int16x8_t a, int16x8_t b) {
  int16x8_t ret;
#if defined(_SIMD_ARM_NEON)
  ret = vminq_s16(a, b);
#elif defined(_SIMD_X86_SSE4_1)
  ret = _mm_min_epi16(a, b);
#else
  for (size_t i = 0; i < 8; ++i) {
    int16_t ai = a.s16[i];
    int16_t bi = b.s16[i];
    ret.s16[i] = ai < b ? ai : bi;
  }
#endif
  return ret;
}

// [scalar] min(a, s)
static inline int16x8_t int16x8_mins(int16x8_t a, int16_t s) {
  return int16x8_min(a, int16x8_inits(s));
}

// [vector] max(a, b)
static inline int16x8_t int16x8_max(int16x8_t a, int16x8_t b) {
  int16x8_t ret;
#if defined(_SIMD_ARM_NEON)
  ret = vmaxq_s16(a, b);
#elif defined(_SIMD_X86_SSE4_1)
  ret = _mm_max_epi16(a, b);
#else
  for (size_t i = 0; i < 8; ++i) {
    int16_t ai = a.s16[i];
    int16_t bi = b.s16[i];
    ret.s16[i] = ai < bi ? bi : ai;
  }
#endif
  return ret;
}

// [scalar] max(a, s)
static inline int16x8_t int16x8_maxs(int16x8_t a, int16_t s) {
  return int16x8_max(a, int16x8_inits(s));
}

// [vector] clamp(a, min, max)
static inline int16x8_t int16x8_clamp(int16x8_t a, int16x8_t min, int16x8_t max) {
  int16x8_t ret;
#if defined(_SIMD_ARM_NEON)
  ret = vminq_s16(vmaxq_s16(a, min), max);
#elif defined(_SIMD_X86_SSE4_1)
  ret = _mm_min_epi16(_mm_max_epi16(a, min), max);
#else
  for (size_t i = 0; i < 8; ++i) {
    int16_t ai = a.s16[i];
    int16_t mini = min.s16[i];
    int16_t maxi = max.s16[i];
    ret.u16[i] = ai < mini ? mini : (ai < maxi ? ai : maxi);
  }
#endif
  return ret;
}

// [scalar] clamp(a, min, max)
static inline int16x8_t int16x8_clamps(int16x8_t a, int16_t min, int16_t max) {
  return int16x8_clamp(a, int16x8_inits(min), int16x8_inits(max));
}

#undef _SIMD_CF

#ifdef __cplusplus
}
#endif

#endif  // _SIMD_INT16X8_H
