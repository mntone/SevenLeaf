#ifndef _SNLF_PLANE_GEOMETRY_HPP
#define _SNLF_PLANE_GEOMETRY_HPP

#include <array>
#include <compositor/CpsrGraphics.h>
#include <compositor/vector/float32x2_t.h>

#include "../utils/SnlfMath.hpp"

namespace sevenleaf {
namespace geometry {
namespace internal {

template <std::size_t W, std::size_t H, bool texcoord = false, std::size_t W1 = W + 1>
struct PlaneGeometryCalculator {
  static_assert(W >= 1);
  static_assert(H >= 1);

  std::array<simd_float32x2_t, (texcoord ? 2 : 1) * W1 * (H + 1)> vertex;
  std::array<uint16x3_t, 2 * W * H> index;

  constexpr PlaneGeometryCalculator(float width, float height, float ox = 0.F, float oy = 0.F, std::size_t oi = 0): vertex(), index() {
    float widthPerSeg = width / W;
    float heightPerSeg = height / H;
    
    std::size_t idx = 0;
    
    std::size_t i = 0, j = 0;
    for (; j <= H; ++j) {
      float y = j * heightPerSeg + oy;
      for (i = 0; i <= W; ++i) {
        float x = i * widthPerSeg + ox;
        vertex[idx++] = simd_float32x2_set(x, y);
        if constexpr (texcoord) {
          vertex[idx++] = simd_float32x2_set(static_cast<float>(i) / W, 1.F - (static_cast<float>(j) / H));
        }
      }
    }

    for (j = 0; j < H; ++j) {
      std::size_t offset = oi + W1 * j;
      for (i = 0; i < W; ++i) {
        uint16_t a = (uint16_t)(offset      + i);
        uint16_t b = (uint16_t)(offset      + i + 1);
        uint16_t c = (uint16_t)(offset + W1 + i);
        uint16_t d = (uint16_t)(offset + W1 + i + 1);
        index[2 * (W * j + i)    ] = {a, b, c};
        index[2 * (W * j + i) + 1] = {c, b, d};
      }
    }
  }

  constexpr std::size_t vertexBufferSize() const noexcept { return vertex.size() * sizeof(simd_float32x2_t); }

  constexpr std::size_t indexBufferSize() const noexcept { return index.size() * sizeof(uint16x3_t); }
};

}  // namespace internal

template <std::size_t W, std::size_t H, bool texcoord = false, std::size_t kIndexCount = 6 * W * H>
class PlaneGeometry final {
public:
  constexpr PlaneGeometry(): vertexBuffer_(nullptr), indexBuffer_(nullptr) {}
  constexpr PlaneGeometry(CpsrDevice const *device, float width, float height) {
    Init(device, width, height);
  }

  PlaneGeometry(PlaneGeometry const& other) = delete;
  PlaneGeometry& operator=(PlaneGeometry const& other) = delete;

  PlaneGeometry(PlaneGeometry&& other) = default;
  PlaneGeometry& operator=(PlaneGeometry&& other) noexcept = default;

  ~PlaneGeometry() noexcept {
    if (vertexBuffer_) {
      CpsrBufferDestroy(vertexBuffer_);
    }
    if (indexBuffer_) {
      CpsrBufferDestroy(indexBuffer_);
    }
  }

  constexpr void Init(CpsrDevice const *device, float width, float height) {
    internal::PlaneGeometryCalculator<W, H, texcoord> geometry(width, height);
    vertexBuffer_ = CpsrBufferCreateFromData(device,
                                             reinterpret_cast<const void *>(geometry.vertex.data()),
                                             geometry.vertexBufferSize(),
                                             CPSR_VERTEX_BUFFER);
    indexBuffer_ = CpsrBufferCreateFromData(
        device, reinterpret_cast<const void *>(geometry.index.data()), geometry.indexBufferSize(), CPSR_INDEX_BUFFER);
  }

  constexpr void Update(float width, float height) {
    internal::PlaneGeometryCalculator<W, H, texcoord> geometry(width, height);
    CpsrBufferWrite(vertexBuffer_, reinterpret_cast<const void *>(geometry.vertex.data()));
    CpsrBufferWrite(indexBuffer_, reinterpret_cast<const void *>(geometry.index.data()));
  }

  constexpr void Draw(CpsrGraphicsContext *context) const noexcept {
    CpsrGraphicsContextSetVertexBuffer(context, 0, vertexBuffer_);
    CpsrGraphicsContextSetIndexBuffer(context, indexBuffer_, CPSR_INDEX_TYPE_UINT16);
    CpsrGraphicsContextDrawIndexed(context, kIndexCount);
  }

  constexpr void DrawInstanced(CpsrGraphicsContext *context, uint32_t instanceCount) const noexcept {
    CpsrGraphicsContextSetVertexBuffer(context, 0, vertexBuffer_);
    CpsrGraphicsContextSetIndexBuffer(context, indexBuffer_, CPSR_INDEX_TYPE_UINT16);
    CpsrGraphicsContextDrawIndexedInstanced(context, kIndexCount, instanceCount, 0, 0);
  }

  constexpr const CpsrBuffer *VertexBuffer() const noexcept { return vertexBuffer_; }

  constexpr const CpsrBuffer *IndexBuffer() const noexcept { return indexBuffer_; }

private:
  CpsrBuffer *vertexBuffer_;
  CpsrBuffer *indexBuffer_;
};

}  // namespace primitives
}  // namespace snlf

#endif  // _SNLF_PLANE_GEOMETRY_HPP
