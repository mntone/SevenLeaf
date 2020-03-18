#ifndef _SIMD_UINT16X8_H
#define _SIMD_UINT16X8_H

#include "simd_base.h"

#ifdef __cplusplus
extern "C" {
#endif

#define _SIMD_CF(__element) (((2 * __element + 1) << 8) | (2 * __element))

// ---
// Constants
// ---
// clang-format off
#define UINT16X8_ZERO uint16x8_inits(0)
#define UINT16X8_ONE  uint16x8_inits(1)
#define UINT16X8_MIN  uint16x8_inits(INT16_MIN)
#define UINT16X8_MAX  uint16x8_inits(INT16_MAX)
// clang-format on

// ---
// Inits
// ---
static inline uint16x8_t uint16x8_t_initv(uint16_t s0,
                                          uint16_t s1,
                                          uint16_t s2,
                                          uint16_t s3,
                                          uint16_t s4,
                                          uint16_t s5,
                                          uint16_t s6,
                                          uint16_t s7) {
  uint16x8_t ret;
#if defined(_SIMD_ARM_NEON)
  ret = { s0, s1, s2, s3, s4, s5, s6, s7 };
#elif defined(_SIMD_X86_SSE2)
  ret = _mm_setr_epi16(s0, s1, s2, s3, s4, s5, s6, s7);
#else
  ret.u16[0] = s0;
  ret.u16[1] = s1;
  ret.u16[2] = s2;
  ret.u16[3] = s3;
  ret.u16[4] = s4;
  ret.u16[5] = s5;
  ret.u16[6] = s6;
  ret.u16[7] = s7;
#endif
  return ret;
}

static inline uint16x8_t uint16x8_inits(uint16_t s) {
  uint16x8_t ret;
#if defined(_SIMD_ARM_NEON)
  ret = vdupq_n_u16(s);
#elif defined(_SIMD_X86_SSE2)
  ret = _mm_set1_epi16(s);
#else
  for (size_t i = 0; i < 8; ++i) {
    ret.u16[i] = s;
  }
#endif
  return ret;
}

static inline uint16x8_t uint16x8_inita(const uint16_t a[]) {
  uint16x8_t ret;
#if defined(_SIMD_ARM_NEON)
  ret = vld1q_u16(a);
#elif defined(_SIMD_X86_SSE2)
  ret = _mm_load_si128((const __m128i *)a);
#else
  for (size_t i = 0; i < 8; ++i) {
    ret.u16[i] = a[i];
  }
#endif
  return ret;
}

static inline uint16x8_t uint16x8_initp(const uint16_t *p) {
  uint16x8_t ret;
#if defined(_SIMD_ARM_NEON)
  ret = vld1q_dup_u16(p);
#elif defined(_SIMD_X86_SSE2)
  ret = _mm_set1_epi16(*p);
#else
  uint16_t s = *p;
  for (size_t i = 0; i < 8; ++i) {
    ret.u16[i] = s;
  }
#endif
  return ret;
}

// ---
// Gets
// ---
static inline uint16_t uint16x8_getat(uint16x8_t v, int index) {
  uint16_t ret;
#if defined(_SIMD_ARM_NEON)
  ret = vst1q_lane_u16(p, v, index);
#elif defined(_SIMD_X86_SSE2)
  switch (index) {
  case 0: ret = (uint16_t)_mm_extract_epi16(v, 0); break;
  case 1: ret = (uint16_t)_mm_extract_epi16(v, 1); break;
  case 2: ret = (uint16_t)_mm_extract_epi16(v, 2); break;
  case 3: ret = (uint16_t)_mm_extract_epi16(v, 3); break;
  case 4: ret = (uint16_t)_mm_extract_epi16(v, 4); break;
  case 5: ret = (uint16_t)_mm_extract_epi16(v, 5); break;
  case 6: ret = (uint16_t)_mm_extract_epi16(v, 6); break;
  case 7: ret = (uint16_t)_mm_extract_epi16(v, 7); break;
  default: unreachable();
  }
#else
  ret = v.u16[index];
#endif
  return ret;
}

static inline void uint16x8_getatp(uint16x8_t v, int index, uint16_t *p) {
#if defined(_SIMD_ARM_NEON)
  vst1q_lane_u16(p, v, index);
#elif defined(_SIMD_X86_SSE2)
  switch (index) {
  case 0: *p = (uint16_t)_mm_extract_epi16(v, 0); break;
  case 1: *p = (uint16_t)_mm_extract_epi16(v, 1); break;
  case 2: *p = (uint16_t)_mm_extract_epi16(v, 2); break;
  case 3: *p = (uint16_t)_mm_extract_epi16(v, 3); break;
  case 4: *p = (uint16_t)_mm_extract_epi16(v, 4); break;
  case 5: *p = (uint16_t)_mm_extract_epi16(v, 5); break;
  case 6: *p = (uint16_t)_mm_extract_epi16(v, 6); break;
  case 7: *p = (uint16_t)_mm_extract_epi16(v, 7); break;
  default: unreachable();
  }
#else
  *p = v.u16[index];
#endif
}

// ---
// Sets
// ---
static inline uint16x8_t uint16x8_setat(uint16x8_t v, int index, uint16_t value) {
  uint16x8_t ret;
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
  ret.u16[index] = value;
#endif
  return ret;
}

// ---
// Packing
// ---
// a0, b0, a1, b1, a2, b2, a3, b3
static inline uint16x8_t uint16x8_unpack16_lo(uint16x8_t a, uint16x8_t b) {
  uint16x8_t ret;
#if defined(_SIMD_ARM_NEON)
  ret = vzipq_u16(a, b).val[0];
#elif defined(_SIMD_X86_SSE2)
  ret = _mm_unpacklo_epi16(a, b);
#else
  for (size_t i = 0; i < 4; ++i) {
    ret.u16[2 * i] = a.u16[i];
    ret.u16[2 * i + 1] = b.u16[i];
  }
#endif
  return ret;
}

// a4, b4, a5, b5, a6, b6, a7, b7
static inline uint16x8_t uint16x8_unpack16_hi(uint16x8_t a, uint16x8_t b) {
  uint16x8_t ret;
#if defined(_SIMD_ARM_NEON)
  ret = vzipq_u16(a, b).val[1];
#elif defined(_SIMD_X86_SSE2)
  ret = _mm_unpackhi_epi16(a, b);
#else
  for (size_t i = 0; i < 4; ++i) {
    ret.u16[2 * i] = a.u16[4 + i];
    ret.u16[2 * i + 1] = b.u16[4 + i];
  }
#endif
  return ret;
}

// a0, b0, a1, b1, a2, b2, a3, b3 | a4, b4, a5, b5, a6, b6, a7, b7
static inline uint16x8x2_t uint16x8_unpack16(uint16x8_t a, uint16x8_t b) {
  uint16x8x2_t ret;
#if defined(_SIMD_ARM_NEON)
  ret = vzipq_u16(a.v, b.v);
#elif defined(_SIMD_X86_SSE2)
  ret = uint16x8x2_init(_mm_unpacklo_epi16(a, b), _mm_unpackhi_epi16(a, b));
#else
  for (size_t i = 0; i < 4; ++i) {
    ret.val[0].u16[2 * i] = a.u16[i];
    ret.val[0].u16[2 * i + 1] = b.u16[i];
  }
  for (size_t i = 0; i < 4; ++i) {
    ret.val[1].u16[2 * i] = a.u16[4 + i];
    ret.val[1].u16[2 * i + 1] = b.u16[4 + i];
  }
#endif
  return ret;
}

// a0, a1, b0, b1, a2, a3, b2, b3
static inline uint16x8_t uint16x8_unpack32_lo(uint16x8_t a, uint16x8_t b) {
  uint16x8_t ret;
#if defined(_SIMD_ARM_NEON)
  ret = ((uint16x8x2_t)vzipq_u32((uint32x4_t)a, (uint32x4_t)b)).val[0];
#elif defined(_SIMD_X86_SSE2)
  ret = _mm_unpacklo_epi32(a, b);
#else
  for (size_t i = 0; i < 2; ++i) {
    ret.u32[2 * i] = a.u32[i];
    ret.u32[2 * i + 1] = b.u32[i];
  }
#endif
  return ret;
}

// a4, a5, b4, b5, a6, a7, b6, b7
static inline uint16x8_t uint16x8_unpack32_hi(uint16x8_t a, uint16x8_t b) {
  uint16x8_t ret;
#if defined(_SIMD_ARM_NEON)
  ret = ((uint16x8x2_t)vzipq_u32((uint32x4_t)a, (uint32x4_t)b)).val[1];
#elif defined(_SIMD_X86_SSE2)
  ret = _mm_unpackhi_epi32(a, b);
#else
  for (size_t i = 0; i < 2; ++i) {
    ret.u32[2 * i] = a.u32[2 + i];
    ret.u32[2 * i + 1] = b.u32[2 + i];
  }
#endif
  return ret;
}

// a0, a1, b0, b1, a2, a3, b2, b3 | a4, a5, b4, b5, a6, a7, b6, b7
static inline uint16x8x2_t uint16x8_unpack32(uint16x8_t a, uint16x8_t b) {
  uint16x8x2_t ret;
#if defined(_SIMD_ARM_NEON)
  ret = (uint16x8x2_t)vzipq_u32((uint32x4_t)a, (uint32x4_t)b);
#elif defined(_SIMD_X86_SSE2)
  ret = uint16x8x2_init(_mm_unpacklo_epi32(a, b), _mm_unpackhi_epi32(a, b));
#else
  for (size_t i = 0; i < 2; ++i) {
    ret.val[0].u32[2 * i] = a.u32[i];
    ret.val[0].u32[2 * i + 1] = b.u32[i];
  }
  for (size_t i = 0; i < 2; ++i) {
    ret.val[1].u32[2 * i] = a.u32[2 + i];
    ret.val[1].u32[2 * i + 1] = b.u32[2 + i];
  }
#endif
  return ret;
}

// a0, a1, a2, a3, b0, b1, b2, b3
static inline uint16x8_t uint16x8_unpack64_lo(uint16x8_t a, uint16x8_t b) {
  uint16x8_t ret;
#if defined(_SIMD_ARM_NEON)
  ret = vcombine_u16(vget_low_u16(a), vget_low_u16(b));
#elif defined(_SIMD_X86_SSE2)
  ret = _mm_unpacklo_epi64(a, b);
#else
  ret.u64[0] = a.u64[0];
  ret.u64[1] = b.u64[0];
#endif
  return ret;
}

// a4, a5, a6, a7, b4, b5, b6, b7
static inline uint16x8_t uint16x8_unpack64_hi(uint16x8_t a, uint16x8_t b) {
  uint16x8_t ret;
#if defined(_SIMD_ARM_NEON)
  ret = vcombine_u16(vget_high_u16(a), vget_high_u16(b));
#elif defined(_SIMD_X86_SSE2)
  ret = _mm_unpackhi_epi64(a, b);
#else
  ret.u64[0] = a.u64[1];
  ret.u64[1] = b.u64[1];
#endif
  return ret;
}

// a0, a1, b0, b1, a2, a3, b2, b3 | a4, a5, a6, a7, b4, b5, b6, b7
static inline uint16x8x2_t uint16x8_unpack64(uint16x8_t a, uint16x8_t b) {
  uint16x8x2_t ret;
#if defined(_SIMD_ARM_NEON)
  ret.val[0] = vcombine_u16(vget_low_u16(a), vget_low_u16(b));
  ret.val[1] = vcombine_u16(vget_high_u16(a), vget_high_u16(b));
#elif defined(_SIMD_X86_SSE2)
  ret = uint16x8x2_init(_mm_unpacklo_epi64(a, b), _mm_unpackhi_epi64(a, b));
#else
  ret.val[0].u64[0] = a.u64[0];
  ret.val[0].u64[1] = b.u64[0];
  ret.val[1].u64[0] = a.u64[1];
  ret.val[1].u64[1] = b.u64[1];
#endif
  return ret;
}

// a1, a0, a3, a2, a5, a4, a7, a6
static inline uint16x8_t uint16x8_reverse32p16(uint16x8_t a) {
  uint16x8_t ret;
#if defined(_SIMD_ARM_NEON)
  ret = vrev32q_u16(a);
#elif defined(_SIMD_X86_SSSE3)
  __m128i mask = _mm_setr_epi8(0x2, 0x3, 0x0, 0x1, 0x6, 0x7, 0x4, 0x5, 0xA, 0xB, 0x8, 0x9, 0xE, 0xF, 0xC, 0xD);
  ret = _mm_shuffle_epi8(a, mask);
#elif defined(_SIMD_X86_SSE2)
  ret = _mm_shufflehi_epi16(_mm_shufflelo_epi16(a, _MM_SHUFFLE(2, 3, 0, 1)), _MM_SHUFFLE(2, 3, 0, 1));
#else
  for (size_t i = 0; i < 8; i += 2) {
    ret.u16[i] = a.u16[i + 1];
    ret.u16[i + 1] = a.u16[i];
  }
#endif
  return ret;
}

// a3, a2, a1, a0, a7, a6, a5, a4
static inline uint16x8_t uint16x8_reverse64p16(uint16x8_t a) {
  uint16x8_t ret;
#if defined(_SIMD_ARM_NEON)
  ret = vrev64q_u16(a);
#elif defined(_SIMD_X86_SSSE3)
  __m128i mask = _mm_setr_epi8(0x6, 0x7, 0x4, 0x5, 0x2, 0x3, 0x0, 0x1, 0xE, 0xF, 0xC, 0xD, 0xA, 0xB, 0x8, 0x9);
  ret = _mm_shuffle_epi8(a, mask);
#elif defined(_SIMD_X86_SSE2)
  ret = _mm_shufflehi_epi16(_mm_shufflelo_epi16(a, _MM_SHUFFLE(3, 2, 1, 0)), _MM_SHUFFLE(3, 2, 1, 0));
#else
  for (size_t i = 0; i < 8; i += 4) {
    ret.u16[i] = a.u16[i + 3];
    ret.u16[i + 1] = a.u16[i + 2];
    ret.u16[i + 2] = a.u16[i + 1];
    ret.u16[i + 3] = a.u16[i];
  }
#endif
  return ret;
}

// a7, a6, a5, a4, a3, a2, a1, a0
static inline uint16x8_t uint16x8_reverse128p16(uint16x8_t a) {
  uint16x8_t ret;
#if defined(_SIMD_ARM_NEON)
  uint16x8_t ar = uint16x8_reverse64p16(a);
  ret = vcombine_u16(vget_high_u16(ar), vget_low_u16(ar));
#elif defined(_SIMD_X86_SSSE3)
  __m128i mask = _mm_setr_epi8(0xE, 0xF, 0xC, 0xD, 0xA, 0xB, 0x8, 0x9, 0x6, 0x7, 0x4, 0x5, 0x2, 0x3, 0x0, 0x1);
  ret = _mm_shuffle_epi8(a, mask);
#elif defined(_SIMD_X86_SSE2)
  ret = _mm_shuffle_epi32(uint16x8_reverse64p16(a), _MM_SHUFFLE(1, 0, 3, 2));
#else
  for (size_t i = 0; i < 8; ++i) {
    ret.u16[i] = a.u16[7 - i];
  }
#endif
  return ret;
}

// a2, a3, a0, a1, a6, a7, a4, a5
static inline uint16x8_t uint16x8_reverse64p32(uint16x8_t a) {
  uint16x8_t ret;
#if defined(_SIMD_ARM_NEON)
  ret = (uint16x8_t)vrev64q_u32((uint32x4_t)a);
#elif defined(_SIMD_X86_SSSE3)
  __m128i mask = _mm_setr_epi8(0x4, 0x5, 0x6, 0x7, 0x0, 0x1, 0x2, 0x3, 0xC, 0xD, 0xE, 0xF, 0x8, 0x9, 0xA, 0xB);
  ret = _mm_shuffle_epi8(a, mask);
#elif defined(_SIMD_X86_SSE2)
  ret = _mm_shufflehi_epi16(_mm_shufflelo_epi16(a, _MM_SHUFFLE(1, 0, 3, 2)), _MM_SHUFFLE(1, 0, 3, 2));
#else
  for (size_t i = 0; i < 8; i += 4) {
    ret.u16[i] = a.u16[i + 2];
    ret.u16[i + 1] = a.u16[i + 3];
    ret.u16[i + 2] = a.u16[i];
    ret.u16[i + 3] = a.u16[i + 1];
  }
#endif
  return ret;
}

// a6, a7, a4, a5, a2, a3, a0, a1
static inline uint16x8_t uint16x8_reverse128p32(uint16x8_t a) {
  uint16x8_t ret;
#if defined(_SIMD_ARM_NEON)
  uint16x8_t ar = uint16x8_reverse64p32(a);
  ret = vcombine_u16(vget_high_u16(ar), vget_low_u16(ar));
#elif defined(_SIMD_X86_SSSE3)
  __m128i mask = _mm_setr_epi8(0xC, 0xD, 0xE, 0xF, 0x8, 0x9, 0xA, 0xB, 0x4, 0x5, 0x6, 0x7, 0x0, 0x1, 0x2, 0x3);
  ret = _mm_shuffle_epi8(a, mask);
#elif defined(_SIMD_X86_SSE2)
  ret = _mm_shuffle_epi32(uint16x8_reverse64p32(a), _MM_SHUFFLE(1, 0, 3, 2));
#else
  for (size_t i = 0; i < 4; ++i) {
    ret.u32[i] = a.u32[3 - i];
  }
#endif
  return ret;
}

// a4, a5, a6, a7, a0, a1, a2, a3
static inline uint16x8_t uint16x8_reverse128p64(uint16x8_t a) {
  uint16x8_t ret;
#if defined(_SIMD_ARM_NEON)
  ret = vcombine_u16(vget_high_u16(a), vget_low_u16(a));
#elif defined(_SIMD_X86_SSSE3)
  __m128i mask = _mm_setr_epi8(0x8, 0x9, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF, 0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7);
  ret = _mm_shuffle_epi8(a, mask);
#elif defined(_SIMD_X86_SSE2)
  ret = _mm_shuffle_epi32(a, _MM_SHUFFLE(1, 0, 3, 2));
#else
  ret.u64[0] = a.u64[1];
  ret.u64[1] = a.u64[0];
#endif
  return ret;
}

// c0, c1, c2, c3, c4, c5, c6, c7
#ifdef _SIMD_X86_SSE4_1
#define uint16x8_blend(__a, __b, __mask) _mm_blend_epi16(__a, __b, __mask);
#else
static inline uint16x8_t uint16x8_blend(uint16x8_t a, uint16x8_t b, int mask) {
  uint16x8_t ret;
#if defined(_SIMD_ARM_NEON)
  uint16x8_t m = { mask & 0x01 ? 0xFFFF : 0, mask & 0x02 ? 0xFFFF : 0, mask & 0x04 ? 0xFFFF : 0,
                   mask & 0x08 ? 0xFFFF : 0, mask & 0x10 ? 0xFFFF : 0, mask & 0x20 ? 0xFFFF : 0,
                   mask & 0x40 ? 0xFFFF : 0, mask & 0x80 ? 0xFFFF : 0 };
  ret = vbslq_u16(m, a, b);
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
    ret.u16[2 * i] = a.u16[i];
    ret.u16[2 * i + 1] = b.u16[i];
  }
#endif
  return ret;
}
#endif

// a5, a6, a7, b0, b1, b2, b3, b4
#if defined(_SIMD_ARM_NEON)
#define uint16x8_alignr(__a, __b, __align) vextq_u16(__b, __a, __align);
#elif defined(_SIMD_X86_SSSE3)
#define uint16x8_alignr(__a, __b, __align) _mm_alignr_epi8(__a, __b, 2 * __align);
#else
static inline uint16x8_t uint16x8_alignr(uint16x8_t a, uint16x8_t b, int align) {
  uint16x8_t ret;
#if defined(_SIMD_X86_SSE2)
  ret = _mm_or_si128(_mm_slli_si128(a, 16 - 2 * align), _mm_srli_si128(b, 2 * align));
#else
  for (size_t i = 0; i < align; ++i) {
    ret.u16[i] = a.u16[8 - i];
  }
  for (size_t i = align; i < 8; ++i) {
    ret.u16[1] = b.u16[i - align];
  }
  return ret;
#endif
}
#endif

// swizzle
static inline uint16x8_t
uint16x8_swizzle(uint16x8_t a, size_t e0, size_t e1, size_t e2, size_t e3, size_t e4, size_t e5, size_t e6, size_t e7) {
  uint16x8_t ret;
#if defined(_SIMD_ARM_NEON)
  uint8x8x2_t ref;
  ref.val[0] = (uint8x8_t)vget_low_u16(a);
  ref.val[1] = (uint8x8_t)vget_high_u16(a);

  uint8x8_t retlo =
      vtbl2_u8(ref, vcreate_u8(_SIMD_CF(e0) | (_SIMD_CF(e1) << 16) | (_SIMD_CF(e2) << 32) | (_SIMD_CF(e3) << 48)));
  uint8x8_t rethi =
      vtbl2_u8(ref, vcreate_u8(_SIMD_CF(e4) | (_SIMD_CF(e5) << 16) | (_SIMD_CF(e6) << 32) | (_SIMD_CF(e7) << 48)));
  ret = vcombine_u16((uint16x4_t)retlo, (uint16x4_t)rethi);
#elif defined(_SIMD_X86_SSSE3)
  __m128i m = _mm_set_epi64x(_SIMD_CF(e4) | (_SIMD_CF(e5) << 16) | (_SIMD_CF(e6) << 32) | (_SIMD_CF(e7) << 48),
                             _SIMD_CF(e0) | (_SIMD_CF(e1) << 16) | (_SIMD_CF(e2) << 32) | (_SIMD_CF(e3) << 48));
  ret = _mm_shuffle_epi8(a, m);
#elif defined(_SIMD_X86_SSE2)
  const __simd128 *ap = (__simd128 *)&a;
  __simd128 *rp = (__simd128 *)&ret;
  rp->u16[0] = ap->u16[e0];
  rp->u16[1] = ap->u16[e1];
  rp->u16[2] = ap->u16[e2];
  rp->u16[3] = ap->u16[e3];
  rp->u16[4] = ap->u16[e4];
  rp->u16[5] = ap->u16[e5];
  rp->u16[6] = ap->u16[e6];
  rp->u16[7] = ap->u16[e7];
#else
  const __simd128 *ap = (__simd128 *)&a;
  ret.u16[0] = ap->u16[e0];
  ret.u16[1] = ap->u16[e1];
  ret.u16[2] = ap->u16[e2];
  ret.u16[3] = ap->u16[e3];
  ret.u16[4] = ap->u16[e4];
  ret.u16[5] = ap->u16[e5];
  ret.u16[6] = ap->u16[e6];
  ret.u16[7] = ap->u16[e7];
#endif
  return ret;
}

// ---
// Logic
// ---
// ~a
static inline uint16x8_t uint16x8_not(uint16x8_t a) {
  uint16x8_t ret;
#if defined(_SIMD_ARM_NEON)
  ret = vmvnq_u16(a);
#elif defined(_SIMD_X86_SSE2)
  ret = _simd_mm_not_si128(a);
#else
  ret = __simd128_not(a);
#endif
  return ret;
}

// a & b
static inline uint16x8_t uint16x8_and(uint16x8_t a, uint16x8_t b) {
  uint16x8_t ret;
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
static inline uint16x8_t uint16x8_andnot(uint16x8_t a, uint16x8_t b) {
  uint16x8_t ret;
#if defined(_SIMD_ARM_NEON)
  ret = vbicq_u16(b, a);
#elif defined(_SIMD_X86_SSE2)
  ret = _mm_andnot_si128(a, b);
#else
  ret = __simd128_andnot(a, b);
#endif
  return ret;
}

// a | b
static inline uint16x8_t uint16x8_or(uint16x8_t a, uint16x8_t b) {
  uint16x8_t ret;
#if defined(_SIMD_ARM_NEON)
  ret = vorrq_u16(a, b);
#elif defined(_SIMD_X86_SSE2)
  ret = _mm_or_si128(a, b);
#else
  ret = __simd128_or(a, b);
#endif
  return ret;
}

// ~a | b
static inline uint16x8_t uint16x8_ornot(uint16x8_t a, uint16x8_t b) {
  uint16x8_t ret;
#if defined(_SIMD_ARM_NEON)
  ret = vornq_u16(b, a);
#elif defined(_SIMD_X86_SSE2)
  ret = _mm_or_si128(_simd_mm_not_si128(a), b);
#else
  ret = __simd128_ornot(a, b);
#endif
  return ret;
}

// a ^ b
static inline uint16x8_t uint16x8_xor(uint16x8_t a, uint16x8_t b) {
  uint16x8_t ret;
#if defined(_SIMD_ARM_NEON)
  ret = veorq_u16(a, b);
#elif defined(_SIMD_X86_SSE2)
  ret = _mm_xor_si128(a, b);
#else
  ret = __simd128_xor(a, b);
#endif
  return ret;
}

// (a & ~m) | (b & m)
static inline uint16x8_t uint16x8_sel(uint16x8_t a, uint16x8_t b, uint16x8_t m) {
  uint16x8_t ret;
#if defined(_SIMD_ARM_NEON)
  ret = vbslq_u16(m, a, b);
#elif defined(_SIMD_X86_SSE2)
  ret = _simd_mm_sel_si128(a, b, m);
#else
  ret = __simd128_sel(a, b, m);
#endif
  return ret;
}

// a >> n
static inline uint16x8_t uint16x8_shr(uint16x8_t a, int n) {
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
static inline uint16x8_t uint16x8_shl(uint16x8_t a, int n) {
  uint16x8_t ret;
#if defined(_SIMD_ARM_NEON)
  ret = vshlq_n_u16(a, n);
#elif defined(_SIMD_X86_SSE2)
  ret = _mm_slli_epi16(a, n);
#else
  for (size_t i = 0; i < 8; ++i) {
    ret.u16[i] = a.u16[i] << n;
  }
#endif
  return ret;
}

// ---
// Arithmetic
// ---
// a + b
static inline uint16x8_t uint16x8_add(uint16x8_t a, uint16x8_t b) {
  uint16x8_t ret;
#if defined(_SIMD_ARM_NEON)
  ret = vaddq_u16(a, b);
#elif defined(_SIMD_X86_SSE2)
  ret = _mm_add_epi16(a, b);
#else
  for (size_t i = 0; i < 8; ++i) {
    ret.u16[i] = a.u16[i] + b.u16[i];
  }
#endif
  return ret;
}

// saturate(a + b)
static inline uint16x8_t uint16x8_adds(uint16x8_t a, uint16x8_t b) {
  uint16x8_t ret;
#if defined(_SIMD_ARM_NEON)
  ret = vqaddq_u16(a, b);
#elif defined(_SIMD_X86_SSE2)
  ret = _mm_adds_epu16(a, b);
#else
  for (size_t i = 0; i < 8; ++i) {
    uint16_t ai = a.u16[i];
    uint16_t bi = b.u16[i];
    ret.u16[i] = (ai > UINT16_MAX - bi) ? UINT16_MAX : ai + bi;
  }
#endif
  return ret;
}

// a - b
static inline uint16x8_t uint16x8_sub(uint16x8_t a, uint16x8_t b) {
  uint16x8_t ret;
#if defined(_SIMD_ARM_NEON)
  ret = vsubq_u16(a, b);
#elif defined(_SIMD_X86_SSE2)
  ret = _mm_sub_epi16(a, b);
#else
  for (size_t i = 0; i < 8; ++i) {
    ret.u16[i] = a.u16[i] - b.u16[i];
  }
#endif
  return ret;
}

// saturate(a - b)
static inline uint16x8_t uint16x8_subs(uint16x8_t a, uint16x8_t b) {
  uint16x8_t ret;
#if defined(_SIMD_ARM_NEON)
  ret = vqsubq_u16(a, b);
#elif defined(_SIMD_X86_SSE2)
  ret = _mm_subs_epu16(a, b);
#else
  for (size_t i = 0; i < 8; ++i) {
    uint16_t ai = a.u16[i];
    uint16_t bi = b.u16[i];
    ret.u16[i] = ai < bi ? 0 : ai - bi;
  }
#endif
  return ret;
}

// a * b
static inline uint16x8_t uint16x8_mul(uint16x8_t a, uint16x8_t b) {
  uint16x8_t ret;
#if defined(_SIMD_ARM_NEON)
  ret = vmulq_u16(a, b);
#elif defined(_SIMD_X86_SSE2)
  ret = _mm_mullo_epi16(a, b);
#else
  for (size_t i = 0; i < 8; ++i) {
    ret.u16[i] = a.u16[i] * b.u16[i];
  }
#endif
  return ret;
}

// ---
// Compare
// ---
// a == b
static inline uint16x8_t uint16x8_cmpeq(uint16x8_t a, uint16x8_t b) {
  uint16x8_t ret;
#if defined(_SIMD_ARM_NEON)
  ret = vceqq_u16(a, b);
#elif defined(_SIMD_X86_SSE2)
  ret = _mm_cmpeq_epi16(a, b);
#else
  for (size_t i = 0; i < 8; ++i) {
    ret.u16[i] = a.u16[i] == b.u16[i] ? 0xFFFF : 0;
  }
#endif
  return ret;
}

// a != b
static inline uint16x8_t uint16x8_cmpne(uint16x8_t a, uint16x8_t b) {
  uint16x8_t ret;
#if defined(_SIMD_ARM_NEON)
  ret = vcneq_u16(a, b);
#elif defined(_SIMD_X86_SSE2)
  ret = _simd_mm_not_si128(_mm_cmpeq_epi16(a, b));
#else
  for (size_t i = 0; i < 8; ++i) {
    ret.u16[i] = a.u16[i] != b.u16[i] ? 0xFFFF : 0;
  }
#endif
  return ret;
}

// a > b
static inline uint16x8_t uint16x8_cmpgt(uint16x8_t a, uint16x8_t b) {
  uint16x8_t ret;
#if defined(_SIMD_ARM_NEON)
  ret = vcgtq_u16(a, b);
#elif defined(_SIMD_X86_SSE4_1)
  ret = _simd_mm_not_si128(_mm_cmpeq_epi16(_mm_min_epu16(a, b), a));
#else
  for (size_t i = 0; i < 8; ++i) {
    ret.u16[i] = a.u16[i] > b.u16[i] ? 0xFFFF : 0;
  }
#endif
  return ret;
}

// a >= b
static inline uint16x8_t uint16x8_cmpge(uint16x8_t a, uint16x8_t b) {
  uint16x8_t ret;
#if defined(_SIMD_ARM_NEON)
  ret = vcgeq_u16(a, b);
#elif defined(_SIMD_X86_SSE4_1)
  ret = _mm_cmpeq_epi16(_mm_max_epu16(a, b), a);
#else
  for (size_t i = 0; i < 8; ++i) {
    ret.u16[i] = a.u16[i] >= b.u16[i] ? 0xFFFF : 0;
  }
#endif
  return ret;
}

// a < b
static inline uint16x8_t uint16x8_cmplt(uint16x8_t a, uint16x8_t b) {
  uint16x8_t ret;
#if defined(_SIMD_ARM_NEON)
  ret = vcltq_u16(a, b);
#elif defined(_SIMD_X86_SSE4_1)
  ret = _simd_mm_not_si128(_mm_cmpeq_epi16(_mm_max_epu16(a, b), a));
#else
  for (size_t i = 0; i < 8; ++i) {
    ret.u16[i] = a.u16[i] < b.u16[i] ? 0xFFFF : 0;
  }
#endif
  return ret;
}

// a <= b
static inline uint16x8_t uint16x8_cmple(uint16x8_t a, uint16x8_t b) {
  uint16x8_t ret;
#if defined(_SIMD_ARM_NEON)
  ret = vcleq_u16(a, b);
#elif defined(_SIMD_X86_SSE4_1)
  ret = _mm_cmpeq_epi16(_mm_min_epu16(a, b), a);
#else
  for (size_t i = 0; i < 8; ++i) {
    ret.u16[i] = a.u16[i] <= b.u16[i] ? 0xFFFF : 0;
  }
#endif
  return ret;
}

// [vector] min(a, b)
static inline uint16x8_t uint16x8_min(uint16x8_t a, uint16x8_t b) {
  uint16x8_t ret;
#if defined(_SIMD_ARM_NEON)
  ret = vminq_u16(a, b);
#elif defined(_SIMD_X86_SSE4_1)
  ret = _mm_min_epu16(a, b);
#else
  for (size_t i = 0; i < 8; ++i) {
    uint16_t ai = a.u16[i];
    uint16_t bi = b.u16[i];
    ret.u16[i] = ai < bi ? ai : bi;
  }
#endif
  return ret;
}

// [scalar] min(a, s)
static inline uint16x8_t uint16x8_mins(uint16x8_t a, uint16_t s) {
  return uint16x8_min(a, uint16x8_inits(s));
}

// [vector] max(a, b)
static inline uint16x8_t uint16x8_max(uint16x8_t a, uint16x8_t b) {
  uint16x8_t ret;
#if defined(_SIMD_ARM_NEON)
  ret = vmaxq_u16(a, b);
#elif defined(_SIMD_X86_SSE4_1)
  ret = _mm_max_epu16(a, b);
#else
  for (size_t i = 0; i < 8; ++i) {
    uint16_t ai = a.u16[i];
    uint16_t bi = b.u16[i];
    ret.u16[i] = ai < bi ? bi : ai;
  }
#endif
  return ret;
}

// [scalar] max(a, s)
static inline uint16x8_t uint16x8_maxs(uint16x8_t a, uint16_t s) {
  return uint16x8_max(a, uint16x8_inits(s));
}

// [vector] clamp(a, min, max)
static inline uint16x8_t uint16x8_clamp(uint16x8_t a, uint16x8_t min, uint16x8_t max) {
  uint16x8_t ret;
#if defined(_SIMD_ARM_NEON)
  ret = vminq_u16(vmaxq_u16(a, min), max);
#elif defined(_SIMD_X86_SSE4_1)
  ret = _mm_min_epu16(_mm_max_epu16(a, min), max);
#else
  for (size_t i = 0; i < 8; ++i) {
    uint16_t ai = a.u16[i];
    uint16_t mini = min.u16[i];
    uint16_t maxi = max.u16[i];
    ret.u16[i] = ai < mini ? mini : (ai < maxi ? ai : maxi);
  }
#endif
  return ret;
}

// [scalar] clamp(a, min, max)
static inline uint16x8_t uint16x8_clamps(uint16x8_t a, uint16_t min, uint16_t max) {
  return uint16x8_clamp(a, uint16x8_inits(min), uint16x8_inits(max));
}

#undef _SIMD_CF

#ifdef __cplusplus
}
#endif

#endif  // _SIMD_UINT16X8_H
