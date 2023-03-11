#ifndef VEC3_HPP
#define VEC3_HPP

#include "definitions.hpp"
#include "math.hpp"

template <typename T>
struct vec3
{
    T x;
    T y;
    T z;
};

using float3 = vec3<float>;

template <typename T>
[[nodiscard]] FORCE_INLINE constexpr vec3<T> operator+(vec3<T> a)
{
    return a;
}

template <typename T>
[[nodiscard]] FORCE_INLINE constexpr vec3<T> operator-(vec3<T> a)
{
    return {-a.x, -a.y, -a.z};
}

template <typename T>
[[nodiscard]] FORCE_INLINE constexpr vec3<T> operator+(vec3<T> a, vec3<T> b)
{
    return {a.x + b.x, a.y + b.y, a.z + b.z};
}

template <typename T>
[[nodiscard]] FORCE_INLINE constexpr vec3<T> operator-(vec3<T> a, vec3<T> b)
{
    return {a.x - b.x, a.y - b.y, a.z - b.z};
}

template <typename T>
[[nodiscard]] FORCE_INLINE constexpr vec3<T> operator*(vec3<T> a, vec3<T> b)
{
    return {a.x * b.x, a.y * b.y, a.z * b.z};
}

template <typename T>
[[nodiscard]] FORCE_INLINE constexpr vec3<T> operator/(vec3<T> a, vec3<T> b)
{
    return {a.x / b.x, a.y / b.y, a.z / b.z};
}

template <typename T>
[[nodiscard]] FORCE_INLINE constexpr vec3<T> operator*(vec3<T> a, T b)
{
    return {a.x * b, a.y * b, a.z * b};
}

template <typename T>
[[nodiscard]] FORCE_INLINE constexpr vec3<T> operator*(T a, vec3<T> b)
{
    return b * a;
}

template <typename T>
[[nodiscard]] FORCE_INLINE constexpr vec3<T> operator/(vec3<T> a, T b)
{
    return {a.x / b, a.y / b, a.z / b};
}

template <typename T>
FORCE_INLINE constexpr vec3<T> &operator+=(vec3<T> &a, vec3<T> b)
{
    a = a + b;
    return a;
}

template <typename T>
FORCE_INLINE constexpr vec3<T> &operator-=(vec3<T> &a, vec3<T> b)
{
    a = a - b;
    return a;
}

template <typename T>
FORCE_INLINE constexpr vec3<T> &operator*=(vec3<T> &a, vec3<T> b)
{
    a = a * b;
    return a;
}

template <typename T>
FORCE_INLINE constexpr vec3<T> &operator/=(vec3<T> &a, vec3<T> b)
{
    a = a / b;
    return a;
}

template <typename T>
FORCE_INLINE constexpr vec3<T> &operator*=(vec3<T> &a, T b)
{
    a = a * b;
    return a;
}

template <typename T>
FORCE_INLINE constexpr vec3<T> &operator/=(vec3<T> &a, T b)
{
    a = a / b;
    return a;
}

template <typename T>
[[nodiscard]] FORCE_INLINE constexpr T dot(vec3<T> a, vec3<T> b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

template <typename T>
[[nodiscard]] FORCE_INLINE constexpr vec3<T> cross(vec3<T> a, vec3<T> b)
{
    return {
        a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x};
}

template <typename T>
[[nodiscard]] FORCE_INLINE T length(vec3<T> a)
{
    return math::sqrt(dot(a, a));
}

template <typename T>
[[nodiscard]] FORCE_INLINE vec3<T> normalize(vec3<T> a)
{
    return a * (1.0f / length(a));
}

#endif // VEC3_HPP
