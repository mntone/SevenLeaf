#ifndef _CPSR_TYPEDEFS_H
#define _CPSR_TYPEDEFS_H

#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// clang-format off

// ---
// Check arch
// ---
#ifdef _MSC_VER
#  if defined(_M_IX86)
#    define ARCH_INTEL   1
#    define ARCH_INTEL32 1
#    define ARCH_32      1
#  elif defined(_M_X64)
#    define ARCH_INTEL   1
#    define ARCH_INTEL64 1
#    define ARCH_64      1
#  elif defined(_M_ARM)
#    define ARCH_ARM     1
#    define ARCH_ARM32   1
#    define ARCH_32      1
#  elif defined(_M_ARM64)
#    define ARCH_ARM     1
#    define ARCH_ARM64   1
#    define ARCH_64      1
#  endif
#else
#  if defined(__i386__)
#    define ARCH_INTEL   1
#    define ARCH_INTEL32 1
#    define ARCH_32      1
#  elif defined(__x86_64__)
#    define ARCH_INTEL   1
#    define ARCH_INTEL64 1
#    define ARCH_64      1
#  elif defined(__arm__)
#    define ARCH_ARM     1
#    define ARCH_ARM32   1
#    define ARCH_32      1
#  elif defined(__aarch64__)
#    define ARCH_ARM     1
#    define ARCH_ARM64   1
#    define ARCH_64      1
#  endif
#endif

#ifdef _WIN32
#  ifdef libcomposer_EXPORTS
#    define CPSR_EXPORT __declspec(dllexport)
#  else
#    define CPSR_EXPORT __declspec(dllimport)
#  endif
#else
#  ifdef libcomposer_EXPORTS
#    define CPSR_EXPORT __attribute__((visibility("default")))
#  else
#    define CPSR_EXPORT
#  endif
#endif

#if defined(__cplusplus)
#  define CPSR_INLINE inline
#elif defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
#  define CPSR_INLINE inline
#elif defined(_MSC_VER)
#  define CPSR_INLINE __inline
#elif defined(__GNUC__)
#  define CPSR_INLINE __inline__
#else
#  define CPSR_INLINE
#endif
// clang-format on

#define CPSR_API_VERSION 1

#define CPSR_CONSTANT_BUFFER_COUNT 14  // from Dx12: D3D12_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT
#define CPSR_VERTEX_BUFFER_COUNT 16    // from Metal 2 (use 16 slots for constant buffer)

#define CPSR_METAL_CONSTANT_BUFFER_OFFSET 16
#define CPSR_METAL_COMPUTE_SHADER_WRITE_TEXTURE_OFFSET 16

typedef struct {
  uint32_t width;
  uint32_t height;
} CpsrSizeU32;

static CPSR_INLINE bool CpsrSizeU32Equal(CpsrSizeU32 a, CpsrSizeU32 b) {
  return (a.width == b.width) && (a.height == b.height);
}

typedef struct {
  uint8_t location;
  uint8_t length;
} CpsrRange;

#endif  // _CPSR_TYPEDEFS_H
