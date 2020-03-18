#ifndef _SNLF_CIRCLE_GEOMETRY_HPP
#define _SNLF_CIRCLE_GEOMETRY_HPP

#include <array>
#include <compositor/CpsrGraphics.h>
#include <compositor/vector/float32x2_t.h>

#include "../utils/SnlfMath.hpp"

namespace sevenleaf {
namespace geometry {
namespace internal {

template <std::size_t N>
struct CircleGeometryCalculator {
  static_assert(N >= 4);

  std::array<simd_float32x2_t, N + 1> vertex;
  std::array<uint16x3_t, N> index;

  constexpr CircleGeometryCalculator(float radius): vertex(), index() {
    constexpr float radianPerSeg = 2 * math::pi<float> / N;

    std::size_t i = 1;
    vertex[0] = simd_float32x2_setzero();
    while (i <= N) {
      float radian = i * radianPerSeg;
      float c = radius * math::cos(radian);
      float s = radius * math::sin(radian);
      vertex[i++] = simd_float32x2_set(c, s);
    }

    i = 0;
    while (i < N - 1) {
      index[i] = {0, (uint16_t)(i + 2), (uint16_t)(i + 1)};
      ++i;
    }
    index[i] = {0, 1, (uint16_t)(i + 1)};
  }

  constexpr std::size_t VertexBufferSize() const noexcept { return (N + 1) * sizeof(simd_float32x2_t); }

  constexpr std::size_t IndexBufferSize() const noexcept { return N * sizeof(uint16x3_t); }
};

}  // namespace internal

template <std::size_t N = 1, std::size_t kIndexCount = 3 * N>
class CircleGeometry final {
public:
  CircleGeometry(): vertexBuffer_(nullptr), indexBuffer_(nullptr) {}
  CircleGeometry(CpsrDevice const *device, float radius) {
    Init(device, radius);
  }

  CircleGeometry(CircleGeometry const& other) = delete;
  CircleGeometry& operator=(CircleGeometry const& other) = delete;

  CircleGeometry(CircleGeometry&& other) = default;
  CircleGeometry& operator=(CircleGeometry&& other) noexcept = default;

  ~CircleGeometry() noexcept {
    if (vertexBuffer_) {
      CpsrBufferDestroy(vertexBuffer_);
    }
    if (indexBuffer_) {
      CpsrBufferDestroy(indexBuffer_);
    }
  }

  constexpr void Init(CpsrDevice const *device, float radius) {
    internal::CircleGeometryCalculator<N> geometry(radius);
    vertexBuffer_ = CpsrBufferCreateFromData(device,
                                             reinterpret_cast<const void *>(geometry.vertex.data()),
                                             geometry.VertexBufferSize(),
                                             CPSR_VERTEX_BUFFER);
    indexBuffer_ = CpsrBufferCreateFromData(
        device, reinterpret_cast<const void *>(geometry.index.data()), geometry.IndexBufferSize(), CPSR_INDEX_BUFFER);
  }

  constexpr void Draw(CpsrGraphicsContext *context) const noexcept {
    CpsrGraphicsContextSetPrimitiveTopology(context, CPSR_PRIMITIVE_TOPOLOGY_TRIANGLE);
    CpsrGraphicsContextSetVertexBuffer(context, 0, vertexBuffer_);
    CpsrGraphicsContextSetIndexBuffer(context, indexBuffer_, CPSR_INDEX_TYPE_UINT16);
    CpsrGraphicsContextDrawIndexed(context, kIndexCount);
  }

  constexpr void DrawInstanced(CpsrGraphicsContext *context, uint32_t instanceCount) const noexcept {
    CpsrGraphicsContextSetPrimitiveTopology(context, CPSR_PRIMITIVE_TOPOLOGY_TRIANGLE);
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

#endif  // _SNLF_CIRCLE_GEOMETRY_HPP
