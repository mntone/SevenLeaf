#ifndef _SNLF_GRAPHICS_FRAME_H
#define _SNLF_GRAPHICS_FRAME_H

#include <osutil_atomic.h>

#include "SnlfCore.h"
#include "SnlfGraphicsDefines.h"

#ifdef __cplusplus
extern "C" {
#endif

// ---
// Prototype
// ---
typedef struct _SnlfGraphicsFrameAllocator *SnlfGraphicsFrameAllocatorRef;

// ---
// Type
// ---
typedef enum {
  // 4-bit RGB
  SNLF_GRAPHICS_FRAME_RGBX4 = 'R04X',
  SNLF_GRAPHICS_FRAME_RGBA4 = 'R04A',
  SNLF_GRAPHICS_FRAME_XBGR4 = 'XB04',
  SNLF_GRAPHICS_FRAME_ABGR4 = 'AB04',
  
  // 5-bit RGB
  SNLF_GRAPHICS_FRAME_RGB5X1 = 'R05X',
  SNLF_GRAPHICS_FRAME_RGB5A1 = 'R05A',
  SNLF_GRAPHICS_FRAME_BGR5X1 = 'B05X',
  SNLF_GRAPHICS_FRAME_BGR5A1 = 'B05A',
  SNLF_GRAPHICS_FRAME_X1RGB5 = 'XR05',
  SNLF_GRAPHICS_FRAME_A1RGB5 = 'AR05',
  SNLF_GRAPHICS_FRAME_X1BGR5 = 'XB05',
  SNLF_GRAPHICS_FRAME_A1BGR5 = 'AB05',
  
  // 5/6-bit RGB
  SNLF_GRAPHICS_FRAME_R5G6B5,

  // 8-bit RGB
  SNLF_GRAPHICS_FRAME_RGBX8 = 'R08X',
  SNLF_GRAPHICS_FRAME_RGBA8 = 'R08A',
  SNLF_GRAPHICS_FRAME_BGRX8 = 'B08X',
  SNLF_GRAPHICS_FRAME_BGRA8 = 'B08A',
  
  // 10-bit RGB
  SNLF_GRAPHICS_FRAME_RGB10X2 = 'R10X',
  SNLF_GRAPHICS_FRAME_RGB10A2 = 'R10A',
  SNLF_GRAPHICS_FRAME_BGR10X2 = 'B10X', // r210 in BE
  SNLF_GRAPHICS_FRAME_BGR10A2 = 'B10A',
  SNLF_GRAPHICS_FRAME_X2RGB10 = 'XR10',
  SNLF_GRAPHICS_FRAME_A2RGB10 = 'AR10',
  SNLF_GRAPHICS_FRAME_X2BGR10 = 'XB10', // R10l, R10b in BE
  SNLF_GRAPHICS_FRAME_A2BGR10 = 'AB10',
  
  // 16-bit RGB
  SNLF_GRAPHICS_FRAME_RGBX16 = 'R16X',
  SNLF_GRAPHICS_FRAME_RGBA16 = 'R16A',
  
  // 8-bit YUV 4:1:1
  SNLF_GRAPHICS_FRAME_NV11 = 'NV11',
  
  // 8-bit YUV 4:2:0
  SNLF_GRAPHICS_FRAME_NV12 = 'NV12',  // Y/UV (UV is packed)
  SNLF_GRAPHICS_FRAME_NV21 = 'NV21',  // Y/VU (VU is packed)
  SNLF_GRAPHICS_FRAME_I420 = 'I420',  // Y/U/V
  SNLF_GRAPHICS_FRAME_YV12 = 'YV12',  // Y/V/U
  SNLF_GRAPHICS_FRAME_IMC2 = 'IMC2',  // Y/V|U
  SNLF_GRAPHICS_FRAME_IMC3 = 'IMC3',  // Y/U V
  SNLF_GRAPHICS_FRAME_IMC4 = 'IMC4',  // Y/U|V
  
  // 8-bit YUV 4:2:2
  SNLF_GRAPHICS_FRAME_I422 = 'I422',  // Planar
  SNLF_GRAPHICS_FRAME_UYVY = 'UYVY',  // HDYC, 2vuy
  SNLF_GRAPHICS_FRAME_VYUY = 'VYUY',
  SNLF_GRAPHICS_FRAME_YUYV = 'YUYV',  // YUY2, 2YUY, yuvs/f
  SNLF_GRAPHICS_FRAME_YVYU = 'YVYU',
  
  // 8-bit YUV 4:4:4
  SNLF_GRAPHICS_FRAME_I444 = 'I444',  // Planar
  SNLF_GRAPHICS_FRAME_AYUV = 'AYUV',
  
  // 10-bit YUV
  SNLF_GRAPHICS_FRAME_P010 = 'P010',  // Planar, 4:2:0
  SNLF_GRAPHICS_FRAME_P210 = 'P210',  // Planar, 4:2:2 (v210)
  SNLF_GRAPHICS_FRAME_Y010 = 'Y010',  // Packed, 4:2:0
  SNLF_GRAPHICS_FRAME_Y210 = 'Y210',  // Packed, 4:2:2
  SNLF_GRAPHICS_FRAME_Y410 = 'Y410',  // Packed, 4:4:4 (v410)
  
  // 12-bit YUV
  SNLF_GRAPHICS_FRAME_P012 = 'P012',  // Planar, 4:2:0
  SNLF_GRAPHICS_FRAME_P212 = 'P212',  // Planar, 4:2:2
  SNLF_GRAPHICS_FRAME_Y012 = 'Y012',  // Packed, 4:2:0
  SNLF_GRAPHICS_FRAME_Y212 = 'Y212',  // Packed, 4:2:2
  SNLF_GRAPHICS_FRAME_Y412 = 'Y412',  // Packed, 4:4:4
  
  // 16-bit YUV
  SNLF_GRAPHICS_FRAME_P016 = 'P016',  // Planar, 4:2:0
  SNLF_GRAPHICS_FRAME_P216 = 'P216',  // Planar, 4:2:2 (v216)
  SNLF_GRAPHICS_FRAME_Y016 = 'Y016',  // Packed, 4:2:0
  SNLF_GRAPHICS_FRAME_Y216 = 'Y216',  // Packed, 4:2:2
  SNLF_GRAPHICS_FRAME_Y416 = 'Y416',  // Packed, 4:4:4 (v416)
} SnlfGraphicsFrameFormat;

// ---
// Struct
// ---
typedef struct {
  SnlfGraphicsFrameAllocatorRef allocator;
  osutil_atomic_int32_t refCount;
  void *heap;
} SnlfGraphicsFrameHeapData;

#define DEFINE_SNLF_GRAPHICS_FRAME_HEADER \
  SnlfGraphicsFrameHeapData *heapData; \
  osutil_atomic_int32_t     refCount; \
  timestamp_t               timestamp

typedef struct {
  DEFINE_SNLF_GRAPHICS_FRAME_HEADER;
  CpsrTexture2D *frame;
} SnlfGraphicsFrameHeader;

// Normalized (0-1) linear RGB frame
typedef struct {
  DEFINE_SNLF_GRAPHICS_FRAME_HEADER;
  CpsrTexture2D *frame;
  SnlfNormalizedFrameDescriptor descriptor;
} SnlfSDRGraphicsFrame;

// RGB frame
typedef struct {
  DEFINE_SNLF_GRAPHICS_FRAME_HEADER;
  const CpsrTexture2D *frame;
  SnlfRGBFrameDescriptor descriptor;
} SnlfRGBSDRGraphicsFrame;

// Packed YUV frame
typedef struct {
  DEFINE_SNLF_GRAPHICS_FRAME_HEADER;
  const CpsrTexture2D *frame;
  SnlfYUVFrameDescriptor descriptor;
} SnlfPackedYUVSDRGraphicsFrame;

// Planer YUV frame
typedef struct {
  DEFINE_SNLF_GRAPHICS_FRAME_HEADER;
  const CpsrTexture2D *yFrame;
  const CpsrTexture2D *uvFrame;
  SnlfYUVFrameDescriptor descriptor;
} SnlfPlanerYUVSDRGraphicsFrame;

// ---
// Allocator
// ---
SnlfGraphicsFrameAllocatorRef SnlfGraphicsFrameAllocatorInit(const CpsrDevice *device, CpsrHeapType heapType, SnlfGraphicsFrameFormat format, CpsrSizeU32 size);
void SnlfGraphicsFrameAllocatorUninit(SnlfGraphicsFrameAllocatorRef allocator);

SnlfGraphicsFrameFormat SnlfGraphicsFrameAllocatorGetFormat(SnlfGraphicsFrameAllocatorRef allocator);
bool SnlfGraphicsFrameAllocatorSetFormat(SnlfGraphicsFrameAllocatorRef allocator, SnlfGraphicsFrameFormat format);

CpsrSizeU32 SnlfGraphicsFrameAllocatorGetSize(SnlfGraphicsFrameAllocatorRef allocator);
void SnlfGraphicsFrameAllocatorSetSize(SnlfGraphicsFrameAllocatorRef allocator, CpsrSizeU32 size);

// ---
// Functions
// ---
SNLF_EXPORT int32_t SnlfGraphicsFrameAddRef(SnlfGraphicsFrameHeader *graphicsFrame);
SNLF_EXPORT int32_t SnlfGraphicsFrameRelease(SnlfGraphicsFrameHeader *graphicsFrame);
SNLF_EXPORT int32_t SnlfGraphicsFrameGetRefCount(SnlfGraphicsFrameHeader *graphicsFrame);

SNLF_EXPORT void SnlfRGBSDRGraphicsFrameWrite(SnlfRGBSDRGraphicsFrame *graphicsFrame, intptr_t data, size_t bytesPerRow);

// ---
// Clean up
// ---
#undef DEFINE_SNLF_GRAPHICS_FRAME_HEADER

#ifdef __cplusplus
}
#endif

#endif // _SNLF_GRAPHICS_FRAME_H
