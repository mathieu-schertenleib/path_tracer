#ifndef VEC_HPP
#define VEC_HPP

#include "definitions.hpp"
#include "math.hpp"
#include "simd.hpp"

template <typename T>
struct v3
{
    T x;
    T y;
    T z;
};

using f32v3 = v3<f32>;
using vf32v3 = v3<vf32>;

template <typename T>
[[nodiscard]] FORCE_INLINE constexpr v3<T> operator+(v3<T> a)
{
    return a;
}

template <typename T>
[[nodiscard]] FORCE_INLINE constexpr v3<T> operator-(v3<T> a)
{
    return {-a.x, -a.y, -a.z};
}

template <typename T>
[[nodiscard]] FORCE_INLINE constexpr v3<T> operator+(v3<T> a, v3<T> b)
{
    return {a.x + b.x, a.y + b.y, a.z + b.z};
}

template <typename T>
[[nodiscard]] FORCE_INLINE constexpr v3<T> operator-(v3<T> a, v3<T> b)
{
    return {a.x - b.x, a.y - b.y, a.z - b.z};
}

template <typename T>
[[nodiscard]] FORCE_INLINE constexpr v3<T> operator*(v3<T> a, v3<T> b)
{
    return {a.x * b.x, a.y * b.y, a.z * b.z};
}

template <typename T>
[[nodiscard]] FORCE_INLINE constexpr v3<T> operator/(v3<T> a, v3<T> b)
{
    return {a.x / b.x, a.y / b.y, a.z / b.z};
}

template <typename T>
[[nodiscard]] FORCE_INLINE constexpr v3<T> operator+(v3<T> a, T b)
{
    return {a.x + b, a.y + b, a.z + b};
}

template <typename T>
[[nodiscard]] FORCE_INLINE constexpr v3<T> operator+(T a, v3<T> b)
{
    return b + a;
}

template <typename T>
[[nodiscard]] FORCE_INLINE constexpr v3<T> operator-(v3<T> a, T b)
{
    return {a.x - b, a.y - b, a.z - b};
}

template <typename T>
[[nodiscard]] FORCE_INLINE constexpr v3<T> operator-(T a, v3<T> b)
{
    return b - a;
}

template <typename T>
[[nodiscard]] FORCE_INLINE constexpr v3<T> operator*(v3<T> a, T b)
{
    return {a.x * b, a.y * b, a.z * b};
}

template <typename T>
[[nodiscard]] FORCE_INLINE constexpr v3<T> operator*(T a, v3<T> b)
{
    return b * a;
}

template <typename T>
[[nodiscard]] FORCE_INLINE constexpr v3<T> operator/(v3<T> a, T b)
{
    return {a.x / b, a.y / b, a.z / b};
}

template <typename T>
[[nodiscard]] FORCE_INLINE constexpr v3<T> operator/(T a, v3<T> b)
{
    return b / a;
}

template <typename T>
FORCE_INLINE constexpr v3<T> &operator+=(v3<T> &a, v3<T> b)
{
    a = a + b;
    return a;
}

template <typename T>
FORCE_INLINE constexpr v3<T> &operator+=(v3<T> &a, T b)
{
    a = a + b;
    return a;
}

template <typename T>
FORCE_INLINE constexpr v3<T> &operator-=(v3<T> &a, v3<T> b)
{
    a = a - b;
    return a;
}

template <typename T>
FORCE_INLINE constexpr v3<T> &operator-=(v3<T> &a, T b)
{
    a = a - b;
    return a;
}

template <typename T>
FORCE_INLINE constexpr v3<T> &operator*=(v3<T> &a, v3<T> b)
{
    a = a * b;
    return a;
}

template <typename T>
FORCE_INLINE constexpr v3<T> &operator*=(v3<T> &a, T b)
{
    a = a * b;
    return a;
}

template <typename T>
FORCE_INLINE constexpr v3<T> &operator/=(v3<T> &a, v3<T> b)
{
    a = a / b;
    return a;
}

template <typename T>
FORCE_INLINE constexpr v3<T> &operator/=(v3<T> &a, T b)
{
    a = a / b;
    return a;
}

namespace vec
{

template <typename T>
[[nodiscard]] FORCE_INLINE constexpr T dot(v3<T> a, v3<T> b)
{
    return math::fmadd(a.x, b.x, math::fmadd(a.y, b.y, a.z * b.z));
}

template <typename T>
[[nodiscard]] FORCE_INLINE constexpr v3<T> cross(v3<T> a, v3<T> b)
{
    return {math::fmsub(a.y, b.z, a.z * b.y),
            math::fmsub(a.z, b.x, a.x * b.z),
            math::fmsub(a.x, b.y, a.y * b.x)};
}

template <typename T>
[[nodiscard]] FORCE_INLINE T length(v3<T> a)
{
    return math::sqrt(vec::dot(a, a));
}

template <typename T>
[[nodiscard]] FORCE_INLINE v3<T> normalize(v3<T> a)
{
    return a * (T {1.0f} / vec::length(a));
}

} // namespace vec

#endif // VEC_HPP
