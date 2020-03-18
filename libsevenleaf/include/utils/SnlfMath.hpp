#ifndef _SNLF_MATH_HPP
#define _SNLF_MATH_HPP

#include <type_traits>

namespace sevenleaf {
namespace math {
namespace internal {

template <typename FloatingPoint,
          typename std::enable_if_t<std::is_floating_point<FloatingPoint>::value>* = nullptr>
constexpr FloatingPoint abs(FloatingPoint x) {
  return x >= 0 ? x : -x;
}

template <typename T>
constexpr bool equal(T x, T y) {
  return abs(x - y) <= std::numeric_limits<T>::epsilon();
}

template <typename T>
constexpr T taylor_series(T x2, T sum, T sign, T t, T n, std::size_t i) {
  T sum2 = sum + sign * t / n;
  return equal(sum, sum2) ? sum2 : taylor_series(x2, sum2, -sign, t * x2, (i + 1) * i * n, i + 2);
}

}  // namespace internal

template <typename FloatingPoint,
          typename std::enable_if_t<std::is_floating_point<FloatingPoint>::value>* = nullptr>
constexpr auto pi = (FloatingPoint)3.14159265358979323846264338327950288;

template <typename FloatingPoint,
          typename std::enable_if_t<std::is_floating_point<FloatingPoint>::value>* = nullptr>
constexpr FloatingPoint sin(FloatingPoint x) {
  FloatingPoint x2 = x * x;
  return internal::taylor_series(x2, x, x2 * x, FloatingPoint(-1), FloatingPoint(6), 4);
}

template <typename FloatingPoint,
          typename std::enable_if_t<std::is_floating_point<FloatingPoint>::value>* = nullptr>
constexpr FloatingPoint cos(FloatingPoint x) {
  FloatingPoint x2 = x * x;
  return internal::taylor_series(x2, FloatingPoint(1), FloatingPoint(-1), x2, FloatingPoint(2), 3);
}

}  // namespace math
}  // namespace snlf

#endif  // _SNLF_MATH_HPP
