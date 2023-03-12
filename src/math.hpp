#ifndef MATH_HPP
#define MATH_HPP

#include "definitions.hpp"

#include <cmath>

namespace math
{

using std::pow;

using std::sqrt;

[[nodiscard]] constexpr FORCE_INLINE f32 fmadd(f32 a, f32 b, f32 c) noexcept
{
    return a * b + c;
}

[[nodiscard]] constexpr FORCE_INLINE f32 fmsub(f32 a, f32 b, f32 c) noexcept
{
    return a * b - c;
}

[[nodiscard]] constexpr FORCE_INLINE f32 min(f32 a, f32 b) noexcept
{
    return b < a ? b : a;
}

[[nodiscard]] constexpr FORCE_INLINE f32 max(f32 a, f32 b) noexcept
{
    return a < b ? b : a;
}

[[nodiscard]] constexpr FORCE_INLINE f32 clamp(f32 value,
                                               f32 low,
                                               f32 high) noexcept
{
    // return (value < low) ? low : (high < value) ? high : value;
    // Note: this is faster than the above
    return math::min(math::max(value, low), high);
}

} // namespace math

#endif // MATH_HPP
