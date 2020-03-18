#ifndef _SIMD_BASE_H
#define _SIMD_BASE_H

#include <float.h>
#include <math.h>
#include <stddef.h>
#include <stdint.h>

// clang-format off
#ifdef _MSC_VER
#  if (_MSC_VER >= 1800)
#    define __alignas_is_defined 1
#  endif
#  if (_MSC_VER >= 1900)
#    define __alignof_is_defined 1
#  endif
#  define unreachable() __assume(0)
#else
#  include <stdalign.h>
#  define unreachable() __builtin_unreachable()
#endif

#ifdef __alignas_is_defined
#  define simd_alignas(X) alignas(X)
#else
#  pragma message("C++11 alignas unsupported. Falling back to compiler attributes.")
#  ifdef __GNUG__
#    define simd_alignas(X) __attribute__((aligned(X)))
#  elif defined(_MSC_VER)
#    define simd_alignas(X) __declspec(align(X))
#  else
#    error Unknown compiler, unknown alignment attribute.
#  endif
#endif

#ifdef __alignof_is_defined
#  define simd_alignof(X) alignof(x)
#else
#  pragma message("C++11 alignof unsupported. Falling back to compiler attributes.")
#  ifdef __GNUG__
#    define simd_alignof(X) __alignof__(X)
#  elif defined(_MSC_VER)
#    define simd_alignof(X) __alignof(X)
#  else
#    error Unknown compiler, unknown alignment attribute.
#  endif
#endif

#ifdef _MSC_VER
#  if defined(_M_IX86)
#    define _SIMD_X86      1
#    define _SIMD_CALLCONV __vectorcall
#  elif defined(_M_X64)
#    define _SIMD_X86      1
#    define _SIMD_X86_64   1
#    define _SIMD_CALLCONV __vectorcall
#  elif defined(_M_ARM)
#    define _SIMD_ARM      1
#    define _SIMD_ARM_NEON 1
#    define _SIMD_CALLCONV __fastcall
#  elif defined(_M_ARM64)
#    define _SIMD_ARM      1
#    define _SIMD_ARM64    1
#    define _SIMD_ARM_NEON 1
#    define _SIMD_CALLCONV __fastcall
#  endif
#else
#  if defined(__i386__)
#    define _SIMD_X86      1
#    define _SIMD_CALLCONV __attribute__((vectorcall))
#  elif defined(__x86_64__)
#    define _SIMD_X86      1
#    define _SIMD_X86_64   1
#    define _SIMD_CALLCONV __attribute__((vectorcall))
#  elif defined(__arm__)
#    define _SIMD_ARM      1
#    define _SIMD_ARM_NEON 1
#    define _SIMD_CALLCONV __attribute__((aarch64_vector_pcs))
#  elif defined(__aarch64__)
#    define _SIMD_ARM      1
#    define _SIMD_ARM64    1
#    define _SIMD_ARM_NEON 1
#    define _SIMD_CALLCONV __attribute__((aarch64_vector_pcs))
#  endif
#endif

#ifndef _SIMD_ARM_NEON
typedef float float32_t;
typedef double float64_t;
#endif

// ---
// Primitive type
// ---
typedef union {
  int8_t s8[16];
  int16_t s16[8];
  int32_t s32[4];
  int64_t s64[2];
  uint8_t u8[16];
  uint16_t u16[8];
  uint32_t u32[4];
  uint64_t u64[2];
  float32_t f32[4];
  float64_t f64[2];
} __simd128;

//#undef _SIMD_X86
//#define ENABLE_SIMD_AVX2

// ---
// x86/x86-64
// ---
#ifdef _SIMD_X86
#  if defined(ENABLE_SIMD_AVX2)
#    include <immintrin.h>
#    define _SIMD_X86_SSE     1
#    define _SIMD_X86_SSE2    1
#    define _SIMD_X86_SSE3    1
#    define _SIMD_X86_SSSE3   1
#    define _SIMD_X86_SSE4_1  1
#    define _SIMD_X86_SSE4_2  1
#    define _SIMD_X86_AVX     1
#    define _SIMD_X86_AVX2    1
#    define _SIMD_X86_FMA3    1
#  elif defined(ENABLE_SIMD_AVX)
#    include <immintrin.h>
#    define _SIMD_X86_SSE     1
#    define _SIMD_X86_SSE2    1
#    define _SIMD_X86_SSE3    1
#    define _SIMD_X86_SSSE3   1
#    define _SIMD_X86_SSE4_1  1
#    define _SIMD_X86_SSE4_2  1
#    define _SIMD_X86_AVX     1
#  elif defined(DISABLE_SIMD_SSE3ORLATER)
#    include <xmmintrin.h>
#    define _SIMD_X86_SSE     1
#    define _SIMD_X86_SSE2    1
#  else
#    include <nmmintrin.h>
#    define _SIMD_X86_SSE     1
#    define _SIMD_X86_SSE2    1
#    define _SIMD_X86_SSE3    1
#    define _SIMD_X86_SSSE3   1
#    define _SIMD_X86_SSE4_1  1
#    define _SIMD_X86_SSE4_2  1
#  endif
// clang-format on

// 128-bit types
typedef __m128 float16x8_t;
typedef __m128 float32x4_t;
typedef __m128 float64x2_t;

#ifdef _SIMD_X86_SSE2
#define _SIMD_NATIVE_128BIT_INTEGER_TYPES 1
typedef __m128i int8x16_t;
typedef __m128i int16x8_t;
typedef __m128i int32x4_t;
typedef __m128i int64x2_t;

typedef __m128i uint8x16_t;
typedef __m128i uint16x8_t;
typedef __m128i uint32x4_t;
typedef __m128i uint64x2_t;
#endif

static inline __m128i _SIMD_CALLCONV _simd_mm_not_si128(__m128i __a) {
  return _mm_andnot_si128(__a, _mm_set1_epi64x(-1));
}

static inline __m128i _SIMD_CALLCONV _simd_mm_sel_si128(__m128i __a, __m128i __b, __m128i __m) {
  return _mm_or_si128(_mm_andnot_si128(__m, __a), _mm_and_si128(__m, __b));
}

// 256-bit types
#ifdef _SIMD_X86_AVX
#define _SIMD_NATIVE_256BIT_TYPES 1
typedef __m256 float16x8x2_t;
typedef __m256 float32x4x2_t;
typedef __m256 float64x2x2_t;
#endif

#ifdef _SIMD_X86_AVX2
#define _SIMD_NATIVE_256BIT_INTEGER_TYPES 1
typedef __m256i int8x16x2_t;
typedef __m256i int16x8x2_t;
typedef __m256i int32x4x2_t;
typedef __m256i int64x2x2_t;

typedef __m256i uint8x16x2_t;
typedef __m256i uint16x8x2_t;
typedef __m256i uint32x4x2_t;
typedef __m256i uint64x2x2_t;

static inline int16x8x2_t _SIMD_CALLCONV int16x8x2_init(int16x8_t v0, int16x8_t v1) {
  int16x8x2_t ret;
  _mm256_inserti128_si256(ret, v0, 0);
  _mm256_inserti128_si256(ret, v1, 1);
  return ret;
}

static inline uint16x8x2_t _SIMD_CALLCONV uint16x8x2_init(uint16x8_t v0, uint16x8_t v1) {
  uint16x8x2_t ret;
  _mm256_inserti128_si256(ret, v0, 0);
  _mm256_inserti128_si256(ret, v1, 1);
  return ret;
}
#endif

// ---
// ARMv7+ NEON
// ---
#elif defined(_SIMD_ARM_NEON)
#include <arm_neon.h>

#define _SIMD_NATIVE_128BIT_INTEGER_TYPES 1

// ---
// Unsupported archtechture
// ---
#else

// 128-bit types
typedef __simd128 float16x8_t;
typedef __simd128 float32x4_t;
typedef __simd128 float64x2_t;

#endif

#ifndef _SIMD_NATIVE_128BIT_INTEGER_TYPES
// 128-bit integer types
typedef __simd128 int8x16_t;
typedef __simd128 int16x8_t;
typedef __simd128 int32x4_t;
typedef __simd128 int64x2_t;

typedef __simd128 uint8x16_t;
typedef __simd128 uint16x8_t;
typedef __simd128 uint32x4_t;
typedef __simd128 uint64x2_t;

static inline __simd128 __simd128_not(__simd128 __a) {
  __simd128 ret;
#if defined(__LP64__) || defined(__LLP64__) || defined(__ILP64__) || defined(_WIN64)
  for (size_t i = 0; i < 2; ++i) {
    ret.u64[i] = ~__a.u64[i];
  }
#else
  for (size_t i = 0; i < 4; ++i) {
    ret.u32[i] = ~__a.u32[i];
  }
#endif
  return ret;
}

static inline __simd128 __simd128_and(__simd128 __a, __simd128 __b) {
  __simd128 ret;
#if defined(__LP64__) || defined(__LLP64__) || defined(__ILP64__) || defined(_WIN64)
  for (size_t i = 0; i < 2; ++i) {
    ret.u64[i] = __a.u64[i] & __b.u64[i];
  }
#else
  for (size_t i = 0; i < 4; ++i) {
    ret.u32[i] = __a.u32[i] & __b.u32[i];
  }
#endif
  return ret;
}

static inline __simd128 __simd128_andnot(__simd128 __a, __simd128 __b) {
  __simd128 ret;
#if defined(__LP64__) || defined(__LLP64__) || defined(__ILP64__) || defined(_WIN64)
  for (size_t i = 0; i < 2; ++i) {
    ret.u64[i] = ~__a.u64[i] | __b.u64[i];
  }
#else
  for (size_t i = 0; i < 4; ++i) {
    ret.u32[i] = ~__a.u32[i] & __b.u32[i];
  }
#endif
  return ret;
}

static inline __simd128 __simd128_or(__simd128 __a, __simd128 __b) {
  __simd128 ret;
#if defined(__LP64__) || defined(__LLP64__) || defined(__ILP64__) || defined(_WIN64)
  for (size_t i = 0; i < 2; ++i) {
    ret.u64[i] = __a.u64[i] | __b.u64[i];
  }
#else
  for (size_t i = 0; i < 4; ++i) {
    ret.u32[i] = __a.u32[i] | __b.u32[i];
  }
#endif
  return ret;
}

static inline __simd128 __simd128_ornot(__simd128 __a, __simd128 __b) {
  __simd128 ret;
#if defined(__LP64__) || defined(__LLP64__) || defined(__ILP64__) || defined(_WIN64)
  for (size_t i = 0; i < 2; ++i) {
    ret.u64[i] = ~__a.u64[i] | __b.u64[i];
  }
#else
  for (size_t i = 0; i < 4; ++i) {
    ret.u32[i] = ~__a.u32[i] | __b.u32[i];
  }
#endif
  return ret;
}

static inline __simd128 __simd128_xor(__simd128 __a, __simd128 __b) {
  __simd128 ret;
#if defined(__LP64__) || defined(__LLP64__) || defined(__ILP64__) || defined(_WIN64)
  for (size_t i = 0; i < 2; ++i) {
    ret.u64[i] = __a.u64[i] ^ __b.u64[i];
  }
#else
  for (size_t i = 0; i < 4; ++i) {
    ret.u32[i] = __a.u32[i] ^ __b.u32[i];
  }
#endif
  return ret;
}

static inline __simd128 __simd128_sel(__simd128 __a, __simd128 __b, __simd128 __m) {
  __simd128 ret;
#if defined(__LP64__) || defined(__LLP64__) || defined(__ILP64__) || defined(_WIN64)
  for (size_t i = 0; i < 2; ++i) {
    ret.u64[i] = (__a.u64[i] & ~__m.u64[i]) | (__b.u64[i] & __m.u64[i]);
  }
#else
  for (size_t i = 0; i < 4; ++i) {
    ret.u64[i] = (__a.u32[i] & ~__m.u32[i]) | (__b.u32[i] & __m.u32[i]);
  }
#endif
  return ret;
}
#endif

// ---
// Primitive data types
// ---
#ifndef _SIMD_ARM_NEON
#ifndef _SIMD_NATIVE_256BIT_TYPES
typedef struct {
  float32x4_t val[2];
} float32x4x2_t;
#endif

typedef struct {
  float32x4_t val[4];
} float32x4x4_t;

#ifndef _SIMD_NATIVE_256BIT_INTEGER_TYPES
typedef struct {
  int16x8_t val[2];
} int16x8x2_t;

static inline int16x8x2_t _SIMD_CALLCONV int16x8x2_init(int16x8_t v0, int16x8_t v1) {
  int16x8x2_t ret;
  ret.val[0] = v0;
  ret.val[1] = v1;
  return ret;
}

typedef struct {
  uint16x8_t val[2];
} uint16x8x2_t;

static inline uint16x8x2_t _SIMD_CALLCONV uint16x8x2_init(uint16x8_t v0, uint16x8_t v1) {
  uint16x8x2_t ret;
  ret.val[0] = v0;
  ret.val[1] = v1;
  return ret;
}
#endif
#endif

// ---
// Data types
// ---
typedef struct {
  uint32_t x : 10;
  uint32_t y : 10;
  uint32_t z : 10;
  uint8_t w : 2;
} uint10x3_2_t;

typedef struct {
  uint8_t w : 2;
  uint32_t x : 10;
  uint32_t y : 10;
  uint32_t z : 10;
} uint2_10x3_t;

// ---
// 2 Data types
// ---
// clang-format off
typedef struct {    int8_t x, y; }   int8x2_t;
typedef struct {   int16_t x, y; }   int16x2_t;
typedef struct {   int32_t x, y; }   int32x2_t;
typedef struct {   uint8_t x, y; }  uint8x2_t;
typedef struct {  uint16_t x, y; }  uint16x2_t;
typedef struct {  uint32_t x, y; }  uint32x2_t;
typedef struct { float32_t x, y; } float32x2_t;
// clang-format on

// ---
// 3 Data types
// ---
// clang-format off
typedef struct {    int8_t x, y, z; }   int8x3_t;
typedef struct {   int16_t x, y, z; }   int16x3_t;
typedef struct {   int32_t x, y, z; }   int32x3_t;
typedef struct {   int64_t x, y, z; }   int64x3_t;
typedef struct {   uint8_t x, y, z; }  uint8x3_t;
typedef struct {  uint16_t x, y, z; }  uint16x3_t;
typedef struct {  uint32_t x, y, z; }  uint32x3_t;
typedef struct {  uint64_t x, y, z; }  uint64x3_t;
typedef struct { float32_t x, y, z; } float32x3_t;
typedef struct { float64_t x, y, z; } float64x3_t;
// clang-format on

#endif  // _SIMD_BASE_H
