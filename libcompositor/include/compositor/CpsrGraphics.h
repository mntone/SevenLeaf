#ifndef _CPSR_GRAPHICS_H
#define _CPSR_GRAPHICS_H

#include "compositor/CpsrTypedefs.h"

#ifdef __cplusplus
extern "C" {
#endif

// ---
// CpsrPixelFormat
// ---
#define DEFPF(__PIXELFORMAT__) CPSR_PIXELFORMAT_##__PIXELFORMAT__
// 15       12 11       8  7  6  5  4  3:0
// 64 32 16 8  SRGB X X FT SI UI SN UN ID
// clang-format off
typedef enum {
  DEFPF(UNKNOWN) = 0,
  
  // 8-bit
  DEFPF(R8_UNORM)         = 0x1011,
  DEFPF(R8_SNORM)         = 0x1021,
  DEFPF(R8_UINT)          = 0x1041,
  DEFPF(R8_SINT)          = 0x1081,
  
  DEFPF(A8_UNORM)         = 0x1012,
  
  // 16-bit
  DEFPF(R16_UNORM)        = 0x2011,
  DEFPF(R16_SNORM)        = 0x2021,
  DEFPF(R16_UINT)         = 0x2041,
  DEFPF(R16_SINT)         = 0x2081,
  DEFPF(R16_FLOAT)        = 0x2101,
  
  DEFPF(RG8_UNORM)        = 0x2012,
  DEFPF(RG8_SNORM)        = 0x2022,
  DEFPF(RG8_UINT)         = 0x2042,
  DEFPF(RG8_SINT)         = 0x2082,
  
  DEFPF(ABGR4_UNORM)      = 0x2013,
  DEFPF(B5G6R5_UNORM)     = 0x2014,
  DEFPF(A1BGR5_UNORM)     = 0x2015,
  DEFPF(BGR5A1_UNORM)     = 0x2016,
  
  // 32-bit
  DEFPF(R32_UINT)         = 0x4041,
  DEFPF(R32_SINT)         = 0x4081,
  DEFPF(R32_FLOAT)        = 0x4101,
  
  DEFPF(RG16_UNORM)       = 0x4012,
  DEFPF(RG16_SNORM)       = 0x4022,
  DEFPF(RG16_UINT)        = 0x4042,
  DEFPF(RG16_SINT)        = 0x4082,
  DEFPF(RG16_FLOAT)       = 0x4102,
  
  DEFPF(RGBA8_UNORM)      = 0x4013,
  DEFPF(RGBA8_UNORM_SRGB) = 0x4813,
  DEFPF(RGBA8_SNORM)      = 0x4023,
  DEFPF(RGBA8_UINT)       = 0x4043,
  DEFPF(RGBA8_SINT)       = 0x4083,
  
  DEFPF(BGRA8_UNORM)      = 0x4014,
  DEFPF(BGRA8_UNORM_SRGB) = 0x4814,
  
  DEFPF(RGB10A2_UNORM)    = 0x4015,
  DEFPF(RGB10A2_UINT)     = 0x4045,
  
  DEFPF(BGR10A2_UNORM)    = 0x4016,
  
  // 64-bit
  DEFPF(RG32_UINT)        = 0x8041,
  DEFPF(RG32_SINT)        = 0x8081,
  DEFPF(RG32_FLOAT)       = 0x8101,
  
  DEFPF(RGBA16_UNORM)     = 0x8012,
  DEFPF(RGBA16_SNORM)     = 0x8022,
  DEFPF(RGBA16_UINT)      = 0x8042,
  DEFPF(RGBA16_SINT)      = 0x8082,
  DEFPF(RGBA16_FLOAT)     = 0x8102,
} CpsrPixelFormat;
// clang-format on
#undef DEFPF

typedef struct {
  bool load : 1;
  bool sample : 1;
  bool mipmap : 1;
  bool renderTarget : 1;
  bool blend : 1;
  bool write : 1;
  bool multisampleLoad : 1;
  bool multisampleSample : 1;
  bool multisampleResolve : 1;
} CpsrPixelFormatCapabilities;

typedef enum {
  CPSR_COLORSPACE_DEFAULT,

  CPSR_COLORSPACE_SRGB,
  CPSR_COLORSPACE_SRGB_LINEAR,

  CPSR_COLORSPACE_ADOBERGB,

  CPSR_COLORSPACE_BT709,

  CPSR_COLORSPACE_DISPLAYP3,
  CPSR_COLORSPACE_DISPLAYP3_HLG,
  CPSR_COLORSPACE_DISPLAYP3_PQ,

  CPSR_COLORSPACE_BT2020,
  CPSR_COLORSPACE_BT2020_HLG,
  CPSR_COLORSPACE_BT2020_PQ,
} CpsrColorSpace;

// ---
// CpsrViewHost
// ---
typedef void *CpsrViewHandle;
struct _CpsrViewHost {
  CpsrViewHandle handle;
  CpsrSizeU32 size;
  CpsrPixelFormat pixelFormat;
};
typedef struct _CpsrViewHost CpsrViewHost;


typedef enum {
  CPSR_DRIVER_DIRECTX12,
  CPSR_DRIVER_METAL2,
} CpsrDriverType;
CPSR_EXPORT CpsrDriverType CpsrGetDriverType();

// ---
// CpsrDevice
// ---
typedef struct _CpsrDevice CpsrDevice;

typedef void (*CpsrDeviceProcedure)(CpsrDevice *device);


CPSR_EXPORT void CpsrEnumDevice(CpsrDeviceProcedure enumFunc);
CPSR_EXPORT CpsrDevice *CpsrDeviceGetDefault();
CPSR_EXPORT CpsrDevice *CpsrDeviceGetLowPower();
#if defined(__APPLE__) && defined(CGDIRECTDISPLAY_H_)
CPSR_EXPORT CpsrDevice *CpsrDeviceGetPreferred(CGDirectDisplayID displayID);
#endif
CPSR_EXPORT void CpsrDeviceDestroy(CpsrDevice *device);
CPSR_EXPORT size_t CpsrDeviceGetName(const CpsrDevice *device, char *deviceName, size_t maxCount);
CPSR_EXPORT uint64_t CpsrDeviceGetCurrentAllocatedSize(const CpsrDevice *device);
CPSR_EXPORT bool CpsrDeviceIsUnifiedMemoryAccess(const CpsrDevice *device);
CPSR_EXPORT bool CpsrDeviceIsMultisampleSupport(const CpsrDevice *device,
                                                CpsrPixelFormat pixelFormat,
                                                uint8_t sampleCount);
CPSR_EXPORT CpsrPixelFormatCapabilities CpsrDeviceGetPixelFormatCapabilities(const CpsrDevice *device,
                                                                             CpsrPixelFormat pixelFormat);

typedef struct {
  uint32_t bufferOffsetAlignment;
  uint8_t maximumRenderTargetCount;
} CpsrDeviceCapabilities;
CPSR_EXPORT CpsrDeviceCapabilities CpsrDeviceGetCapabilities(const CpsrDevice *device);

#if defined(__APPLE__) && defined(MTL_EXPORT)
CPSR_EXPORT id<MTLDevice> CpsrDeviceGetNativeHandle(const CpsrDevice *device);
#endif

// ---
// CpsrFence
// ---
typedef enum {
  CPSR_FENCE_DEFAULT,
  CPSR_FENCE_SHARED,
} CpsrFenceType;
typedef struct _CpsrFence CpsrFence;

CPSR_EXPORT CpsrFence *CpsrFenceCreate(const CpsrDevice *device, CpsrFenceType fenceType);
CPSR_EXPORT void CpsrFenceDestroy(CpsrFence *fence);

// ---
// Cpsr data descriptor
// ---
typedef enum {
  CPSR_TEXTURE_USAGE_NONE = 0,
  CPSR_TEXTURE_USAGE_READ = 1 << 0,
  CPSR_TEXTURE_USAGE_WRITE = 1 << 1,
  CPSR_TEXTURE_USAGE_RENDER_TARGET = 1 << 2,
} CpsrTextureUsage;
typedef struct {
  CpsrSizeU32 size;
  uint16_t arrayLength;
  CpsrPixelFormat pixelFormat;
  CpsrTextureUsage usage;
} CpsrTexture2DDescriptor;

// ---
// CpsrHeap
// ---
typedef enum {
  CPSR_HEAP_TYPE_DEFAULT,
  CPSR_HEAP_TYPE_UPLOAD = 1 << 0,
  CPSR_HEAP_TYPE_READBACK = 1 << 1,
} CpsrHeapType;
typedef struct _CpsrHeap CpsrHeap;

CPSR_EXPORT CpsrHeap *CpsrHeapCreate(const CpsrDevice *device, size_t size, CpsrHeapType type);
CPSR_EXPORT CpsrHeap *CpsrHeapCreateFormTexture2DDescriptor(const CpsrDevice *device,
                                                            const CpsrTexture2DDescriptor *descriptor,
                                                            uint8_t count,
                                                            CpsrHeapType type);
CPSR_EXPORT CpsrHeap *CpsrHeapCreateFormTexture2DDescriptors(const CpsrDevice *device,
                                                             const CpsrTexture2DDescriptor *descriptors,
                                                             uint8_t maxCount,
                                                             CpsrHeapType type);
CPSR_EXPORT void CpsrHeapDestroy(CpsrHeap *heap);

// ---
// CpsrBuffer
// ---
typedef enum {
  CPSR_CONSTANT_BUFFER,
  CPSR_VERTEX_BUFFER,
  CPSR_INDEX_BUFFER,
} CpsrBufferType;
typedef enum {
  CPSR_INDEX_TYPE_UINT16,
  CPSR_INDEX_TYPE_UINT32,
} CpsrIndexType;
typedef struct _CpsrBuffer CpsrBuffer;

CPSR_EXPORT CpsrBuffer *CpsrBufferCreateFromSize(const CpsrDevice *device,
                                                 size_t sizeInBytes,
                                                 CpsrHeapType heapType,
                                                 CpsrBufferType bufferType);
CPSR_EXPORT CpsrBuffer *CpsrBufferCreateFromData(const CpsrDevice *device,
                                                 const void *data,
                                                 size_t sizeInBytesCpsr,
                                                 CpsrBufferType bufferType);
CPSR_EXPORT void CpsrBufferDestroy(CpsrBuffer *buffer);

CPSR_EXPORT size_t CpsrBufferGetSize(const CpsrBuffer *buffer);
CPSR_EXPORT size_t CpsrBufferGetCapacity(const CpsrBuffer *buffer);
CPSR_EXPORT void CpsrBufferSetName(const CpsrBuffer *buffer, const char *name, size_t maxCount);

CPSR_EXPORT bool CpsrBufferMap(const CpsrBuffer *buffer, void **data);
CPSR_EXPORT void CpsrBufferUnmap(const CpsrBuffer *buffer, CpsrRange range);

CPSR_EXPORT bool CpsrBufferRead(const CpsrBuffer *buffer, void *data);
CPSR_EXPORT bool CpsrBufferWrite(CpsrBuffer *buffer, const void *data);

#ifdef NDEBUG
#define CpsrBufferSetNameD(__BUFFER__, __NAME__)
#else
#define CpsrBufferSetNameD(__BUFFER__, __NAME__) CpsrBufferSetName(__BUFFER__, __NAME__, strlen(__NAME__))
#endif

// ---
// CpsrTexture2D
// ---
typedef struct _CpsrTexture2D CpsrTexture2D;

CPSR_EXPORT CpsrTexture2D *CpsrTexture2DCreate(const CpsrDevice *device,
                                               const CpsrTexture2DDescriptor *desc,
                                               CpsrHeapType heapType);
CPSR_EXPORT CpsrTexture2D *CpsrTexture2DCreateFromHeap(const CpsrHeap *heap, const CpsrTexture2DDescriptor *desc);
#if defined(__APPLE__) && defined(IOSURFACE_REF_H)
CPSR_EXPORT CpsrTexture2D *CpsrTexture2DCreateFromIOSurface(const CpsrDevice *device,
                                                            IOSurfaceRef ioSurface,
                                                            CpsrPixelFormat pixelFormat);
#endif
#if defined(__APPLE__) && defined(MTL_EXPORT)
CPSR_EXPORT CpsrTexture2D *CpsrTexture2DCreateFromNativeHandle(const CpsrDevice *device, id<MTLTexture> native);
#endif
CPSR_EXPORT void CpsrTexture2DDestroy(CpsrTexture2D *texture2D);

CPSR_EXPORT CpsrSizeU32 CpsrTexture2DGetSize(const CpsrTexture2D *texture2D);
CPSR_EXPORT size_t CpsrTexture2DGetLength(const CpsrTexture2D *texture2D);

CPSR_EXPORT bool CpsrTexture2DWrite(const CpsrTexture2D *texture2D, const void *data, size_t bytesPerRow);

// ---
// CpsrSwapChain
// ---
typedef struct _CpsrCommandQueue CpsrCommandQueue;
typedef struct _CpsrSwapChain CpsrSwapChain;

CPSR_EXPORT CpsrSwapChain *CpsrSwapChainCreate(const CpsrCommandQueue *graphicsCommandQueue,
                                               CpsrViewHost viewHost,
                                               uint8_t bufferCount,
                                               bool vsyncEnable);
CPSR_EXPORT void CpsrSwapChainDestroy(CpsrSwapChain *swapChain);

CPSR_EXPORT bool CpsrSwapChainNextBuffer(CpsrSwapChain *swapChain);

CPSR_EXPORT CpsrSizeU32 CpsrSwapChainGetSize(CpsrSwapChain *swapChain);
CPSR_EXPORT void CpsrSwapChainSetSize(CpsrSwapChain *swapChain, CpsrSizeU32 size);

CPSR_EXPORT uint8_t CpsrSwapChainGetBufferCount(CpsrSwapChain *swapChain);
CPSR_EXPORT void CpsrSwapChainSetBufferCount(CpsrSwapChain *swapChain, uint8_t bufferCount);

CPSR_EXPORT void CpsrSwapChainSetColorSpace(CpsrSwapChain *swapChain, CpsrColorSpace colorSpace);

// ---
// CpsrShaderLibrary
// ---
typedef struct _CpsrShaderLibrary CpsrShaderLibrary;

CPSR_EXPORT CpsrShaderLibrary *CpsrShaderLibraryCreate(const CpsrDevice *device, const char *filePath);
CPSR_EXPORT void CpsrShaderLibraryDestroy(CpsrShaderLibrary *shaderLibrary);

// ---
// CpsrShaderFunction
// ---
typedef enum {
  CPSR_SHADER_TYPE_MASK = 0x000000FF,
#if defined(_WIN32) || defined(USE_ALL_SHADER_LANGUAGE)
  CPSR_SHADER_TYPE_VERTEX_SHADER = 1,
  CPSR_SHADER_TYPE_PIXEL_SHADER = 2,
  CPSR_SHADER_TYPE_COMPUTE_SHADER = 4,
#endif

  CPSR_SHADER_LANGUAGE_MASK = 0xFF000000,
  CPSR_SHADER_HLSL = (0x1 << 24),
  CPSR_SHADER_MSL = (0x2 << 24),

  CPSR_SHADER_VERSION_MASK = 0x00FFFF00,

#if defined(_WIN32) || defined(USE_ALL_SHADER_LANGUAGE)
  CPSR_SHADER_VERSION_HLSL5_0 = CPSR_SHADER_HLSL | (5 << 16),
  CPSR_SHADER_VERSION_HLSL5_1 = CPSR_SHADER_HLSL | (5 << 16) | (1 << 8),
  CPSR_SHADER_VERSION_HLSL6_0 = CPSR_SHADER_HLSL | (6 << 16),
  CPSR_SHADER_VERSION_HLSL6_1 = CPSR_SHADER_HLSL | (6 << 16) | (1 << 8),
#endif

#if defined(__APPLE__) || defined(USE_ALL_SHADER_LANGUAGE)
#if TARGET_OS_IPHONE
  CPSR_SHADER_VERSION_MSL1_0 = CPSR_SHADER_MSL | (1 << 16),
#endif
  CPSR_SHADER_VERSION_MSL1_1 = CPSR_SHADER_MSL | (1 << 16) | (1 << 8),
  CPSR_SHADER_VERSION_MSL1_2 = CPSR_SHADER_MSL | (1 << 16) | (2 << 8),
  CPSR_SHADER_VERSION_MSL2_0 = CPSR_SHADER_MSL | (2 << 16),
  CPSR_SHADER_VERSION_MSL2_1 = CPSR_SHADER_MSL | (2 << 16) | (1 << 8),
  CPSR_SHADER_VERSION_MSL2_2 = CPSR_SHADER_MSL | (2 << 16) | (2 << 8),
#endif
} CpsrShaderType;
typedef struct _CpsrShaderFunction CpsrShaderFunction;

CPSR_EXPORT CpsrShaderFunction *CpsrShaderFunctionCreateFromLibrary(const CpsrShaderLibrary *shaderLibrary,
                                                                    const char *functionName);
CPSR_EXPORT CpsrShaderFunction *CpsrShaderFunctionCreateFromString(const CpsrDevice *device,
                                                                   const char *functionString,
                                                                   CpsrShaderType shaderType);
CPSR_EXPORT void CpsrShaderFunctionDestroy(CpsrShaderFunction *shaderFunction);

// ---
// CpsrCommandQueue
// ---
CPSR_EXPORT CpsrCommandQueue *CpsrCommandQueueCreate(const CpsrDevice *device);
CPSR_EXPORT void CpsrCommandQueueDestroy(CpsrCommandQueue *commandQueue);

// ---
// CpsrCommandBuffer
// ---
typedef struct _CpsrCommandBuffer CpsrCommandBuffer;

CPSR_EXPORT CpsrCommandBuffer *CpsrCommandBufferCreate(const CpsrCommandQueue *commandQueue);
CPSR_EXPORT void CpsrCommandBufferDestroy(CpsrCommandBuffer *commandBuffer);

CPSR_EXPORT void CpsrCommandBufferExecute(const CpsrCommandBuffer *commandBuffer);
CPSR_EXPORT void CpsrCommandBufferExecuteAndPresent(const CpsrCommandBuffer *commandBuffer,
                                                    const CpsrSwapChain *wapChain);

CPSR_EXPORT void CpsrCommandBufferSingle(const CpsrCommandBuffer *commandBuffer,
                                         const CpsrFence *fence,
                                         uint64_t value);
CPSR_EXPORT void CpsrCommandBufferWait(const CpsrCommandBuffer *commandBuffer, const CpsrFence *fence, uint64_t value);

#ifdef NDEBUG
#define CpsrCommandBufferPushDebugGroup(__COMMAND_BUFFER__, __GROUP_NAME__)
#define CpsrCommandBufferPopDebugGroup(__COMMAND_BUFFER__)
#else
CPSR_EXPORT void _CpsrCommandBufferPushDebugGroup(const CpsrCommandBuffer *commandBuffer, const char *groupName);
CPSR_EXPORT void _CpsrCommandBufferPopDebugGroup(const CpsrCommandBuffer *commandBuffer);

#define CpsrCommandBufferPushDebugGroup(__COMMAND_BUFFER__, __GROUP_NAME__) \
  _CpsrCommandBufferPushDebugGroup(__COMMAND_BUFFER__, __GROUP_NAME__)
#define CpsrCommandBufferPopDebugGroup(__COMMAND_BUFFER__) _CpsrCommandBufferPopDebugGroup(__COMMAND_BUFFER__)
#endif

// ---
// CpsrBlendStateDescriptor
// ---
typedef enum {
  CPSR_BLEND_ZERO,
  CPSR_BLEND_ONE,
  CPSR_BLEND_SOURCE_COLOR,
  CPSR_BLEND_ONE_MINUS_SOURCE_COLOR,
  CPSR_BLEND_SOURCE_ALPHA,
  CPSR_BLEND_ONE_MINUS_SOURCE_ALPHA,
  CPSR_BLEND_DESTINATION_COLOR,
  CPSR_BLEND_ONE_MINUS_DESTINATION_COLOR,
  CPSR_BLEND_DESTINATION_ALPHA,
  CPSR_BLEND_ONE_MINUS_DESTINATION_ALPHA,
  CPSR_BLEND_SOURCE_ALPHA_SATURATED,
  CPSR_BLEND_BLEND_FACTOR,
  CPSR_BLEND_ONE_MINUS_BLEND_FACTOR,
  CPSR_BLEND_SOURCE1_COLOR,
  CPSR_BLEND_ONE_MINUS_SOURCE1_COLOR,
  CPSR_BLEND_SOURCE1_ALPHA,
  CPSR_BLEND_ONE_MINUS_SOURCE1_ALPHA,

  CPSR_BLEND_UNKNOWN = 31,
} CpsrBlendFactor;

typedef enum {
  CPSR_BLEND_OPERATION_ADD,
  CPSR_BLEND_OPERATION_SUBTRACT,
  CPSR_BLEND_OPERATION_REVERSE_SUBTRACT,
  CPSR_BLEND_OPERATION_MIN,
  CPSR_BLEND_OPERATION_MAX,
} CpsrBlendOperation;

typedef struct {
  bool blendEnable : 1;
  CpsrBlendFactor srcColorBlend : 5;
  CpsrBlendFactor dstColorBlend : 5;
  CpsrBlendOperation colorOperation : 3;
  CpsrBlendFactor srcAlphaBlend : 5;
  CpsrBlendFactor dstAlphaBlend : 5;
  CpsrBlendOperation alphaOperation : 3;
  union {
    struct {
      bool writeRed : 1;
      bool writeGreen : 1;
      bool writeBlue : 1;
      bool writeAlpha : 1;
    };
    uint8_t writeMask : 4;
  };
} CpsrBlendDescriptor;

static const CpsrBlendDescriptor kCpsrBlendDefault = {
  false,          CPSR_BLEND_ONE,  CPSR_BLEND_ZERO,          CPSR_BLEND_OPERATION_ADD,
  CPSR_BLEND_ONE, CPSR_BLEND_ZERO, CPSR_BLEND_OPERATION_ADD, { { true, true, true, true } }
};

static const CpsrBlendDescriptor kCpsrBlendAlphaStraight = {
  true,           CPSR_BLEND_SOURCE_ALPHA,           CPSR_BLEND_ONE_MINUS_SOURCE_COLOR, CPSR_BLEND_OPERATION_ADD,
  CPSR_BLEND_ONE, CPSR_BLEND_ONE_MINUS_SOURCE_ALPHA, CPSR_BLEND_OPERATION_ADD,          { { true, true, true, true } }
};

static const CpsrBlendDescriptor kCpsrBlendAlphaPremultiplied = { true,
                                                                  CPSR_BLEND_ONE,
                                                                  CPSR_BLEND_ONE_MINUS_SOURCE_COLOR,
                                                                  CPSR_BLEND_OPERATION_ADD,
                                                                  CPSR_BLEND_ONE,
                                                                  CPSR_BLEND_ONE_MINUS_SOURCE_ALPHA,
                                                                  CPSR_BLEND_OPERATION_ADD,
                                                                  { { true, true, true, true } } };

// ---
// CpsrRasterizerStateDescriptor
// ---
typedef enum {
  CPSR_FILL_WIREFRAME,
  CPSR_FILL_SOLID,
} CpsrFillMode;

typedef enum {
  CPSR_CULL_NONE,
  CPSR_CULL_FRONT,
  CPSR_CULL_BACK,
} CpsrCullMode;

typedef enum {
  CPSR_DEPTH_CLIP_CLIP,
  CPSR_DEPTH_CLIP_CLAMP,
} CpsrDepthClipMode;

typedef struct {
  CpsrFillMode fillMode : 2;
  CpsrCullMode cullMode : 2;
  bool counterClockwise : 1;
  CpsrDepthClipMode depthClipMode : 1;
  int depthBias;
  float depthBiasClamp;
  float slopeScaledDepthBias;
  uint8_t rasterSampleCount;
} CpsrRasterizerDescriptor;

static const CpsrRasterizerDescriptor kCpsrRasterizerDefault = {
  CPSR_FILL_SOLID, CPSR_CULL_BACK, false, CPSR_DEPTH_CLIP_CLIP, 0, 0.F, 0.F, 1
};

// ---
// CpsrGraphicsPipelineState
// ---
typedef struct _CpsrGraphicsPipelineState CpsrGraphicsPipelineState;

CPSR_EXPORT CpsrGraphicsPipelineState *CpsrGraphicsPipelineStateCreate(const CpsrDevice *device);
CPSR_EXPORT void CpsrGraphicsPipelineStateDestroy(CpsrGraphicsPipelineState *pipelineState);

CPSR_EXPORT void CpsrGraphicsPipelineStateSetVertexFunction(CpsrGraphicsPipelineState *pipelineState,
                                                            const CpsrShaderFunction *shaderFunction);
CPSR_EXPORT void CpsrGraphicsPipelineStateSetPixelFunction(CpsrGraphicsPipelineState *pipelineState,
                                                           const CpsrShaderFunction *shaderFunction);

CPSR_EXPORT void CpsrGraphicsPipelineStateGetBlendState(const CpsrGraphicsPipelineState *pipelineState,
                                                        uint8_t index,
                                                        CpsrBlendDescriptor *blendState);
CPSR_EXPORT void CpsrGraphicsPipelineStateSetBlendState(CpsrGraphicsPipelineState *pipelineState,
                                                        uint8_t index,
                                                        const CpsrBlendDescriptor *blendState);

CPSR_EXPORT void CpsrGraphicsPipelineStateGetRasterizerState(const CpsrGraphicsPipelineState *pipelineState,
                                                             CpsrRasterizerDescriptor *rasterizerState);
CPSR_EXPORT void CpsrGraphicsPipelineStateSetRasterizerState(CpsrGraphicsPipelineState *pipelineState,
                                                             const CpsrRasterizerDescriptor *rasterizerState);

typedef enum {
  CPSR_PRIMITIVE_TOPOLOGY_TYPE_UNSPECIFIED,
  CPSR_PRIMITIVE_TOPOLOGY_TYPE_POINT,
  CPSR_PRIMITIVE_TOPOLOGY_TYPE_LINE,
  CPSR_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
} CpsrPrimitiveTopologyType;

CPSR_EXPORT CpsrPrimitiveTopologyType
CpsrGraphicsPipelineStateGetPrimitiveTopologyType(const CpsrGraphicsPipelineState *pipelineState);
CPSR_EXPORT void CpsrGraphicsPipelineStateSetPrimitiveTopologyType(CpsrGraphicsPipelineState *pipelineState,
                                                                   CpsrPrimitiveTopologyType primitiveTopologyType);

CPSR_EXPORT uint8_t CpsrGraphicsPipelineStateGetMultisampleCount(const CpsrGraphicsPipelineState *pipelineState);
CPSR_EXPORT void CpsrGraphicsPipelineStateSetMultisampleCount(CpsrGraphicsPipelineState *pipelineState,
                                                              uint8_t multisampleCount);

CPSR_EXPORT void CpsrSetRenderTargetPixelFormat(CpsrGraphicsPipelineState *pipelineState,
                                                uint8_t index,
                                                CpsrPixelFormat pixelFormat);
CPSR_EXPORT void CpsrSetRenderTargetPixelFormatFromTexture2D(CpsrGraphicsPipelineState *pipelineState,
                                                             uint8_t index,
                                                             const CpsrTexture2D *renderTarget);
CPSR_EXPORT void CpsrSetRenderTargetPixelFormatFromSwapChain(CpsrGraphicsPipelineState *pipelineState,
                                                             uint8_t index,
                                                             CpsrSwapChain *swapChain);

// ---
// CpsrGraphicsContext
// ---
typedef struct _CpsrGraphicsContext CpsrGraphicsContext;

CPSR_EXPORT CpsrGraphicsContext *CpsrGraphicsContextCreate(const CpsrCommandBuffer *graphicsContext);
CPSR_EXPORT void CpsrGraphicsContextDestroy(CpsrGraphicsContext *graphicsContext);

typedef struct {
  float red, green, blue, alpha;
} CpsrClearColor;
static const CpsrClearColor kCpsrClearBlack = {
  0.F, 0.F, 0.F, 1.F
};

CPSR_EXPORT void CpsrGraphicsContextClearRenderTarget(CpsrGraphicsContext *graphicsContext,
                                                      uint8_t index,
                                                      CpsrClearColor color);

CPSR_EXPORT void CpsrGraphicsContextSetRenderTargetFromTexture2D(CpsrGraphicsContext *graphicsContext,
                                                                 const CpsrTexture2D *renderTarget);
CPSR_EXPORT void CpsrGraphicsContextSetRenderTargetFromSwapChain(CpsrGraphicsContext *graphicsContext,
                                                                 CpsrSwapChain *swapChain);
CPSR_EXPORT bool CpsrGraphicsContextSetPipelineState(CpsrGraphicsContext *graphicsContext,
                                                     CpsrGraphicsPipelineState *pipelineState);

typedef struct {
  float originX, originY;
  float width, height;
  float znear, zfar;
} CpsrViewport;
CPSR_EXPORT void CpsrGraphicsContextSetViewport(CpsrGraphicsContext *graphicsContext, CpsrViewport viewport);

typedef struct {
  float x, y;
  float width, height;
} CpsrScissorRect;
CPSR_EXPORT void CpsrGraphicsContextSetScissorRect(CpsrGraphicsContext *graphicsContext, CpsrScissorRect scissorRect);

CPSR_EXPORT void CpsrGraphicsContextSetConstantBuffer(CpsrGraphicsContext *graphicsContext,
                                                      uint8_t index,
                                                      const CpsrBuffer *constantBuffer);
CPSR_EXPORT void CpsrGraphicsContextSetConstantBufferWithOffset(CpsrGraphicsContext *graphicsContext,
                                                                uint8_t index,
                                                                const CpsrBuffer *constantBuffer,
                                                                uint32_t offset);
CPSR_EXPORT void CpsrGraphicsContextSetVertexBuffer(CpsrGraphicsContext *graphicsContext,
                                                    uint8_t index,
                                                    const CpsrBuffer *vertexBuffer);
CPSR_EXPORT void CpsrGraphicsContextSetVertexBuffers(CpsrGraphicsContext *graphicsContext,
                                                     CpsrRange range,
                                                     const CpsrBuffer **vertexBuffers);
CPSR_EXPORT void CpsrGraphicsContextSetIndexBuffer(CpsrGraphicsContext *graphicsContext,
                                                   const CpsrBuffer *indexBuffer,
                                                   CpsrIndexType indexType);
CPSR_EXPORT void CpsrGraphicsContextSetIndexBufferWithOffset(CpsrGraphicsContext *graphicsContext,
                                                             const CpsrBuffer *indexBuffer,
                                                             uint32_t offset,
                                                             CpsrIndexType indexType);
CPSR_EXPORT void CpsrGraphicsContextSetTexture(const CpsrGraphicsContext *graphicsContext,
                                               uint8_t index,
                                               const CpsrTexture2D *texture);

typedef enum {
  CPSR_PRIMITIVE_TOPOLOGY_POINT,
  CPSR_PRIMITIVE_TOPOLOGY_LINE,
  CPSR_PRIMITIVE_TOPOLOGY_LINE_STRIP,
  CPSR_PRIMITIVE_TOPOLOGY_TRIANGLE,
  CPSR_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP,
} CpsrPrimitiveTopology;
CPSR_EXPORT void CpsrGraphicsContextSetPrimitiveTopology(CpsrGraphicsContext *graphicsContext,
                                                         CpsrPrimitiveTopology primitiveTopology);

CPSR_EXPORT void CpsrGraphicsContextDraw(const CpsrGraphicsContext *graphicsContext,
                                         uint32_t vertexStart,
                                         uint32_t vertexCount);
CPSR_EXPORT void CpsrGraphicsContextDrawInstanced(const CpsrGraphicsContext *graphicsContext,
                                                  uint32_t vertexStart,
                                                  uint32_t vertexCount,
                                                  uint32_t instanceCount);
CPSR_EXPORT void CpsrGraphicsContextDrawIndexed(const CpsrGraphicsContext *graphicsContext, uint32_t indexCount);
CPSR_EXPORT void CpsrGraphicsContextDrawIndexedInstanced(const CpsrGraphicsContext *graphicsContext,
                                                         uint32_t indexCount,
                                                         uint32_t instanceCount,
                                                         int32_t baseVertex,
                                                         uint32_t baseInstance);
CPSR_EXPORT void CpsrGraphicsContextClose(const CpsrGraphicsContext *graphicsContext);

#ifdef NDEBUG
#define CpsrGraphicsContextPushDebugGroup(__GRAPHICS_CONTEXT__, __GROUP_NAME__)
#define CpsrGraphicsContextPopDebugGroup(__GRAPHICS_CONTEXT__)
#else
CPSR_EXPORT void _CpsrGraphicsContextPushDebugGroup(const CpsrGraphicsContext *graphicsContext, const char *groupName);
CPSR_EXPORT void _CpsrGraphicsContextPopDebugGroup(const CpsrGraphicsContext *graphicsContext);

#define CpsrGraphicsContextPushDebugGroup(__GRAPHICS_CONTEXT__, __GROUP_NAME__) \
  _CpsrGraphicsContextPushDebugGroup(__GRAPHICS_CONTEXT__, __GROUP_NAME__)
#define CpsrGraphicsContextPopDebugGroup(__GRAPHICS_CONTEXT__) _CpsrGraphicsContextPopDebugGroup(__GRAPHICS_CONTEXT__)
#endif

// ---
// CpsrComputePipelineState
// ---
typedef struct _CpsrComputePipelineState CpsrComputePipelineState;

CPSR_EXPORT CpsrComputePipelineState *CpsrComputePipelineStateCreate(const CpsrDevice *device);
CPSR_EXPORT void CpsrComputePipelineStateDestroy(CpsrComputePipelineState *pipelineState);

CPSR_EXPORT void CpsrComputePipelineStateSetFunction(CpsrComputePipelineState *pipelineState,
                                                     const CpsrShaderFunction *shaderFunction);

// ---
// CpsrComputeContext
// ---
typedef struct _CpsrComputeContext CpsrComputeContext;

CPSR_EXPORT CpsrComputeContext *CpsrComputeContextCreate(const CpsrCommandBuffer *commandBuffer);
CPSR_EXPORT void CpsrComputeContextDestroy(CpsrComputeContext *computeContext);

CPSR_EXPORT void CpsrComputeContextSetConstantBuffer(CpsrComputeContext *computeContext,
                                                     uint8_t index,
                                                     const CpsrBuffer *vertexBuffer);
CPSR_EXPORT void CpsrComputeContextSetReadTexture(CpsrComputeContext *computeContext,
                                                  uint8_t index,
                                                  const CpsrTexture2D *texture);
CPSR_EXPORT void CpsrComputeContextSetWriteTexture(CpsrComputeContext *computeContext,
                                                   uint8_t index,
                                                   const CpsrTexture2D *texture);
CPSR_EXPORT void CpsrComputeContextDispatch(const CpsrComputeContext *computeContext,
                                            uint16_t x,
                                            uint16_t y,
                                            uint16_t z);
CPSR_EXPORT void CpsrComputeContextClose(const CpsrComputeContext *computeContext);

#ifdef NDEBUG
#define CpsrComputeContextPushDebugGroup(__COMPUTE_CONTEXT__, __GROUP_NAME__)
#define CpsrComputeContextPopDebugGroup(__COMPUTE_CONTEXT__)
#else
CPSR_EXPORT void _CpsrComputeContextPushDebugGroup(const CpsrComputeContext *commandContext, const char *groupName);
CPSR_EXPORT void _CpsrComputeContextPopDebugGroup(const CpsrComputeContext *commandContext);

#define CpsrComputeContextPushDebugGroup(__COMPUTE_CONTEXT__, __GROUP_NAME__) \
  _CpsrComputeContextPushDebugGroup(__COMPUTE_CONTEXT__, __GROUP_NAME__)
#define CpsrComputeContextPopDebugGroup(__COMPUTE_CONTEXT__) _CpsrComputeContextPopDebugGroup(__COMPUTE_CONTEXT__)
#endif

#ifdef __cplusplus
}
#endif

#endif  // _CPSR_GRAPHICS_H
