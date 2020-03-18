#ifndef _SNLF_RECTANGLE_GEOMETRY_HPP
#define _SNLF_RECTANGLE_GEOMETRY_HPP

#include <array>
#include <compositor/CpsrGraphics.h>
#include <compositor/vector/float32x2_t.h>

#include "../utils/SnlfMath.hpp"

namespace sevenleaf {
namespace geometry {
namespace internal {

template <std::size_t W, std::size_t H, std::size_t W1 = W + 1, std::size_t H1 = H + 1>
struct RectangleGeometryCalculator {
  static_assert(W >= 1);
  static_assert(H >= 1);

  std::array<simd_float32x2_t, 2 * (W1 * 2 + 2 * H1)> vertex;
  std::array<uint16x3_t, 2 * ((2 * W) + (2 * H))> index;

  constexpr RectangleGeometryCalculator(float width, float height, float borderSize = 1.F): vertex(), index() {
    float width2 = width - borderSize;
    float height2 = height - borderSize;
    float widthPerSeg = width2 / W;
    float heightPerSeg = height2 / H;

    std::size_t idx = 0;

    // Top
    for (std::size_t i = 0; i <= W; ++i) {
      vertex[idx++] = simd_float32x2_set(i * widthPerSeg, 0);
    }
    for (std::size_t i = 0; i <= W; ++i) {
      vertex[idx++] = simd_float32x2_set(i * widthPerSeg, borderSize);
    }

    // Right
    for (std::size_t i = 0; i <= H; ++i) {
      float y = i * heightPerSeg;
      vertex[idx++] = simd_float32x2_set(width2, y);
      vertex[idx++] = simd_float32x2_set(width, y);
    }

    // Bottom
    for (std::size_t i = 0; i <= W; ++i) {
      vertex[idx++] = simd_float32x2_set(borderSize + i * widthPerSeg, height2);
    }
    for (std::size_t i = 0; i <= W; ++i) {
      vertex[idx++] = simd_float32x2_set(borderSize + i * widthPerSeg, height);
    }

    // Left
    for (std::size_t i = 0; i <= H; ++i) {
      float y = borderSize + i * heightPerSeg;
      vertex[idx++] = simd_float32x2_set(0, y);
      vertex[idx++] = simd_float32x2_set(borderSize, y);
    }

    // Index
    std::size_t i = 0, j = 0, k = 0, o1 = 0;
    idx = 0;
    for (; k < 4; ++k) {
      for (j = 0; j < H; ++j) {
        std::size_t offset = o1 + W1 * j;
        for (i = 0; i < W; ++i) {
          uint16_t a = (uint16_t)(offset      + i);
          uint16_t b = (uint16_t)(offset      + i + 1);
          uint16_t c = (uint16_t)(offset + W1 + i);
          uint16_t d = (uint16_t)(offset + W1 + i + 1);
          index[idx++] = {a, c, b};
          index[idx++] = {b, c, d};
        }
      }
      if (k % 2 == 0) {
        o1 += W1 * 2;
      } else {
        o1 += 2 * H1;
      }
    }
  }

  constexpr std::size_t vertexBufferSize() const noexcept { return vertex.size() * sizeof(simd_float32x2_t); }

  constexpr std::size_t indexBufferSize() const noexcept { return index.size() * sizeof(uint16x3_t); }
};

}  // namespace internal

template <std::size_t Wn, std::size_t Hn, std::size_t kIndexCount = 6 * (2 * Wn + 2 * Hn)>
class RectangleGeometry final {
public:
  RectangleGeometry(): vertexBuffer_(nullptr), indexBuffer_(nullptr) {}
  RectangleGeometry(CpsrDevice const *device, float width, float height, float borderSize = 1.F) {
    Init(device, width, height, borderSize);
  }

  RectangleGeometry(RectangleGeometry const& other) = delete;
  RectangleGeometry& operator=(RectangleGeometry const& other) = delete;

  RectangleGeometry(RectangleGeometry&& other) = default;
  RectangleGeometry& operator=(RectangleGeometry&& other) noexcept = default;

  ~RectangleGeometry() noexcept {
    if (vertexBuffer_) {
      CpsrBufferDestroy(vertexBuffer_);
    }
    if (indexBuffer_) {
      CpsrBufferDestroy(indexBuffer_);
    }
  }

  constexpr void Init(CpsrDevice const *device, float width, float height, float borderSize = 1.F) {
    internal::RectangleGeometryCalculator<Wn, Hn> geometry(width, height, borderSize);
    vertexBuffer_ = CpsrBufferCreateFromData(device,
                                             reinterpret_cast<const void *>(geometry.vertex.data()),
                                             geometry.vertexBufferSize(),
                                             CPSR_VERTEX_BUFFER);
    indexBuffer_ = CpsrBufferCreateFromData(
        device, reinterpret_cast<const void *>(geometry.index.data()), geometry.indexBufferSize(), CPSR_INDEX_BUFFER);
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

#endif  // _SNLF_RECTANGLE_GEOMETRY_HPP
