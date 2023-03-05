#ifndef VEC3_HPP
#define VEC3_HPP

#include <cmath>

#ifdef _MSC_VER
#define INLINE __forceinline inline
#else
#define INLINE __attribute__((always_inline)) inline
#endif

struct vec3
{
    float x;
    float y;
    float z;
};

[[nodiscard]] INLINE constexpr vec3 operator+(vec3 a) noexcept
{
    return a;
}

[[nodiscard]] INLINE constexpr vec3 operator-(vec3 a) noexcept
{
    return {-a.x, -a.y, -a.z};
}

[[nodiscard]] INLINE constexpr vec3 operator+(vec3 a, vec3 b) noexcept
{
    return {a.x + b.x, a.y + b.y, a.z + b.z};
}

[[nodiscard]] INLINE constexpr vec3 operator-(vec3 a, vec3 b) noexcept
{
    return {a.x - b.x, a.y - b.y, a.z - b.z};
}

[[nodiscard]] INLINE constexpr vec3 operator*(vec3 a, vec3 b) noexcept
{
    return {a.x * b.x, a.y * b.y, a.z * b.z};
}

[[nodiscard]] INLINE constexpr vec3 operator/(vec3 a, vec3 b) noexcept
{
    return {a.x / b.x, a.y / b.y, a.z / b.z};
}

[[nodiscard]] INLINE constexpr vec3 operator*(vec3 a, float b) noexcept
{
    return {a.x * b, a.y * b, a.z * b};
}

[[nodiscard]] INLINE constexpr vec3 operator*(float a, vec3 b) noexcept
{
    return b * a;
}

[[nodiscard]] INLINE constexpr vec3 operator/(vec3 a, float b) noexcept
{
    return {a.x / b, a.y / b, a.z / b};
}

[[nodiscard]] INLINE constexpr float dot(vec3 a, vec3 b) noexcept
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

[[nodiscard]] INLINE constexpr vec3 cross(vec3 a, vec3 b) noexcept
{
    return {
        a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x};
}

[[nodiscard]] INLINE constexpr float length(vec3 a) noexcept
{
    return std::sqrt(dot(a, a));
}

[[nodiscard]] INLINE constexpr vec3 normalize(vec3 a) noexcept
{
    return a * (1.0f / length(a));
}

#endif // VEC3_HPP
