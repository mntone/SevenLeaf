#ifndef _CPSR_GRAPHICS_HPP
#define _CPSR_GRAPHICS_HPP

#include "CpsrGraphics.h"

#ifndef __cplusplus
#error This header requires C++
#endif

namespace compositor {

// ---
// DebugMarker
// ---
class DebugMarker final {
public:
  constexpr DebugMarker(CpsrCommandBuffer const *commandBuffer, char const *name) noexcept
    : commandBuffer_(commandBuffer)
  {
    CpsrCommandBufferPushDebugGroup(commandBuffer, name);
  }
  
  ~DebugMarker() noexcept {
    CpsrCommandBufferPopDebugGroup(commandBuffer_);
  }
  
private:
  DebugMarker(DebugMarker const& other) = delete;
  DebugMarker& operator=(DebugMarker const& other) = delete;
  
  DebugMarker(DebugMarker&& other) = delete;
  DebugMarker& operator=(DebugMarker&& other) noexcept = delete;
  
private:
  CpsrCommandBuffer const *commandBuffer_;
};

// ---
// Buffer
// ---
class Buffer {
public:
  Buffer(CpsrDevice const *device, std::size_t sizeInBytes, CpsrBufferType bufferType, char const *name = nullptr) noexcept
    : native_(CpsrBufferCreateFromSize(device, sizeInBytes, CPSR_HEAP_TYPE_DEFAULT, bufferType))
  {
    if (name) {
      CpsrBufferSetNameD(native_, name);
    }
  }
  
  constexpr Buffer(CpsrBuffer *native, char const *name = nullptr) noexcept
    : native_(native)
  {
    if (name) {
      CpsrBufferSetNameD(native_, name);
    }
  }
  
  Buffer() = delete;
  Buffer(Buffer const& other) = delete;
  Buffer& operator=(Buffer const& other) = delete;
  
  Buffer(Buffer&& other) = default;
  Buffer& operator=(Buffer&& other) noexcept = default;

  ~Buffer() noexcept {
    CpsrBufferDestroy(native_);
  }
  
  inline std::size_t Size() const noexcept {
    return CpsrBufferGetSize(native_);
  }
  
  inline std::size_t Capacity() const noexcept {
    return CpsrBufferGetCapacity(native_);
  }
  
  constexpr operator CpsrBuffer *() const noexcept { return native_; }
  
protected:
  CpsrBuffer *native_;
};

template <typename T,
          CpsrBufferType bufferType,
          typename std::enable_if<std::is_pod<T>::value>::type* = nullptr>
class UploadBuffer final: public Buffer {
public:
  UploadBuffer() noexcept: Buffer(nullptr) {}
  
  UploadBuffer(CpsrDevice const *device, char const *name = nullptr) noexcept
    : Buffer(CpsrBufferCreateFromSize(device, sizeof(T), CPSR_HEAP_TYPE_UPLOAD, bufferType), name)
  {}
  
  constexpr void Init(CpsrDevice const *device, char const *name = nullptr) {
    native_ = CpsrBufferCreateFromSize(device, sizeof(T), CPSR_HEAP_TYPE_UPLOAD, bufferType);
  }
  
  constexpr void Sync() const noexcept {
    CpsrBufferWrite(native_, &data);
  }
  
  constexpr T& Data() noexcept { return data; }
  
  constexpr T *operator&() noexcept { return &data; }
  constexpr operator T& () const noexcept { return data; }

  T data;
};

template <typename T,
          CpsrBufferType bufferType,
          typename std::enable_if<std::is_pod<T>::value>::type* = nullptr>
class ReadBackBuffer final: public Buffer {
public:
  ReadBackBuffer() noexcept: Buffer(nullptr) {}
  
  ReadBackBuffer(CpsrDevice const *device, char const *name = nullptr) noexcept
    : Buffer(CpsrBufferCreateFromSize(device, sizeof(T), CPSR_HEAP_TYPE_READBACK, bufferType), name)
  {}
  
  constexpr void Init(CpsrDevice const *device, char const *name = nullptr) {
    native_ = CpsrBufferCreateFromSize(device, sizeof(T), CPSR_HEAP_TYPE_READBACK, bufferType);
  }
  
  constexpr void Sync() noexcept {
    CpsrBufferRead(native_, &data);
  }
  
  constexpr T& Data() noexcept { return data; }
  
  constexpr T *operator&() noexcept { return &data; }
  constexpr operator T& () const noexcept { return data; }

  T data;
};

// ---
// GraphicsPipelineState
// ---
enum class PrimitiveTopologyType {
  Unspecified = CPSR_PRIMITIVE_TOPOLOGY_TYPE_UNSPECIFIED,
  Point = CPSR_PRIMITIVE_TOPOLOGY_TYPE_POINT,
  Line = CPSR_PRIMITIVE_TOPOLOGY_TYPE_LINE,
  Triangle = CPSR_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
};

class GraphicsPipelineState final {
public:
  constexpr GraphicsPipelineState() noexcept: native_(nullptr) {}
  
  GraphicsPipelineState(CpsrDevice const *device) noexcept
    : native_(CpsrGraphicsPipelineStateCreate(device))
  {}

  ~GraphicsPipelineState() noexcept {
    CpsrGraphicsPipelineStateDestroy(native_);
  }
  
  inline void Init(CpsrDevice const *device) {
    native_ = CpsrGraphicsPipelineStateCreate(device);
  }

  inline void SetVertexFunction(CpsrShaderFunction const *function) noexcept {
    CpsrGraphicsPipelineStateSetVertexFunction(native_, function);
  }

  inline void SetPixelFunction(CpsrShaderFunction const *function) noexcept {
    CpsrGraphicsPipelineStateSetPixelFunction(native_, function);
  }

  inline void SetBlendState(uint8_t index, CpsrBlendDescriptor const& blendState) noexcept {
    CpsrGraphicsPipelineStateSetBlendState(native_, index, &blendState);
  }

  inline void SetRasterizerState(CpsrRasterizerDescriptor const& rasterizerState) noexcept {
    CpsrGraphicsPipelineStateSetRasterizerState(native_, &rasterizerState);
  }

  inline PrimitiveTopologyType PrimitiveTopologyType() const noexcept {
    return static_cast<enum PrimitiveTopologyType>(CpsrGraphicsPipelineStateGetPrimitiveTopologyType(native_));
  }
  inline void SetPrimitiveTopologyType(enum PrimitiveTopologyType primitiveTopologyType) noexcept {
    CpsrGraphicsPipelineStateSetPrimitiveTopologyType(native_, static_cast<CpsrPrimitiveTopologyType>(primitiveTopologyType));
  }

  inline uint8_t MultisampleCount() const noexcept {
    return CpsrGraphicsPipelineStateGetMultisampleCount(native_);
  }
  inline void SetMultisampleCount(uint8_t multisampleCount) noexcept {
    CpsrGraphicsPipelineStateSetMultisampleCount(native_, multisampleCount);
  }
  
  constexpr operator CpsrGraphicsPipelineState *() const noexcept { return native_; }

protected:
  CpsrGraphicsPipelineState *native_;
};

// ---
// GraphicsContext
// ---
enum class IndexType {
  UInt16 = CPSR_INDEX_TYPE_UINT16,
  UInt32 = CPSR_INDEX_TYPE_UINT32,
};
enum class PrimitiveTopology {
  Point = CPSR_PRIMITIVE_TOPOLOGY_POINT,
  Line = CPSR_PRIMITIVE_TOPOLOGY_LINE,
  LineStrip = CPSR_PRIMITIVE_TOPOLOGY_LINE_STRIP,
  Triangle = CPSR_PRIMITIVE_TOPOLOGY_TRIANGLE,
  TriangleStrip = CPSR_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP,
};

class GraphicsContext final {
public:
  constexpr GraphicsContext(CpsrGraphicsContext *native) noexcept
    : native_(native)
  {}
  
  ~GraphicsContext() noexcept {
    CpsrGraphicsContextDestroy(native_);
  }
  
  inline void ClearRenderTarget(uint8_t index, CpsrClearColor color) noexcept {
    CpsrGraphicsContextClearRenderTarget(native_, index, color);
  }
  
  inline void SetRenderTarget(uint8_t index, CpsrTexture2D const *renderTarget) noexcept {
    CpsrGraphicsContextSetRenderTargetFromTexture2D(native_, renderTarget);
  }
  
  inline void SetRenderTarget(uint8_t index, CpsrSwapChain *swapChain) noexcept {
    CpsrGraphicsContextSetRenderTargetFromSwapChain(native_, swapChain);
  }
  
  inline bool SetPipelineState(CpsrGraphicsPipelineState *pipelineState) noexcept {
    return CpsrGraphicsContextSetPipelineState(native_, pipelineState);
  }
  
  inline void SetViewport(CpsrViewport viewport) noexcept {
    CpsrGraphicsContextSetViewport(native_, viewport);
  }
  
  inline void SetScissorRect(CpsrScissorRect scissorRect) noexcept {
    CpsrGraphicsContextSetScissorRect(native_, scissorRect);
  }
  
  inline void SetConstantBuffer(uint8_t index, CpsrBuffer const *constantBuffer) noexcept {
    CpsrGraphicsContextSetConstantBuffer(native_, index, constantBuffer);
  }
  
  inline void SetVertexBuffer(uint8_t index, CpsrBuffer const *vertexBuffer) noexcept {
    CpsrGraphicsContextSetVertexBuffer(native_, index, vertexBuffer);
  }
  
  inline void SetIndexBuffer(CpsrBuffer const *indexBuffer, IndexType indexType) noexcept {
     CpsrGraphicsContextSetIndexBuffer(native_, indexBuffer, static_cast<CpsrIndexType>(indexType));
   }
  
  inline void SetTexture(uint8_t index, CpsrTexture2D const *texture) noexcept {
    CpsrGraphicsContextSetTexture(native_, index, texture);
  }
  
  inline void SetPrimitiveTopology(PrimitiveTopology primitiveTopology) noexcept {
    CpsrGraphicsContextSetPrimitiveTopology(native_, static_cast<CpsrPrimitiveTopology>(primitiveTopology));
  }
  
  inline void Draw(uint32_t vertexStart, uint32_t vertexCount) const noexcept {
    CpsrGraphicsContextDraw(native_, vertexStart, vertexCount);
  }
  
  inline void Draw(uint32_t vertexStart, uint32_t vertexCount, uint32_t instanceCount) const noexcept {
    CpsrGraphicsContextDrawInstanced(native_, vertexStart, vertexCount, instanceCount);
  }
  
  inline void DrawIndexed(uint32_t indexCount) const noexcept {
    CpsrGraphicsContextDrawIndexed(native_, indexCount);
  }
  
  inline void DrawIndexed(uint32_t indexCount, uint32_t instanceCount, int32_t baseVertex, uint32_t baseInstance) const noexcept {
    CpsrGraphicsContextDrawIndexedInstanced(native_, indexCount, instanceCount, baseVertex, baseInstance);
  }
  
  inline void Close() {
    CpsrGraphicsContextClose(native_);
  }
  
  constexpr operator CpsrGraphicsContext *() const noexcept { return native_; }
  
private:
  CpsrGraphicsContext *native_;
};

// ---
// ComputePipelineState
// ---
class ComputePipelineState final {
public:
  ComputePipelineState(CpsrDevice const *device) noexcept
    : native_(CpsrComputePipelineStateCreate(device))
  {}

  ~ComputePipelineState() noexcept {
    CpsrComputePipelineStateDestroy(native_);
  }

  inline void SetFunction(CpsrShaderFunction const *function) noexcept {
    CpsrComputePipelineStateSetFunction(native_, function);
  }

  constexpr CpsrComputePipelineState *Get() const noexcept { return native_; }

protected:
  CpsrComputePipelineState *native_;
};

}

#endif  // _CPSR_GRAPHICS_HPP
