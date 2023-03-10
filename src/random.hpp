#ifndef RANDOM_HPP
#define RANDOM_HPP

#include "definitions.hpp"
#include "float3.hpp"

#include <cmath>
#include <cstdint>
#include <limits>

[[nodiscard]] FORCE_INLINE constexpr float random(std::uint32_t &state) noexcept
{
    state ^= state << 13;
    state ^= state >> 17;
    state ^= state << 5;
    return static_cast<float>(state) *
           (1.0f /
            static_cast<float>(std::numeric_limits<std::uint32_t>::max()));
}

// Returns a random float3 with length <= 1.0
[[nodiscard]] FORCE_INLINE constexpr float3
random_in_sphere(std::uint32_t &state) noexcept
{
    for (;;)
    {
        const float3 v {random(state) * 2.0f - 1.0f,
                      random(state) * 2.0f - 1.0f,
                      random(state) * 2.0f - 1.0f};
        if (dot(v, v) <= 1.0f)
        {
            return v;
        }
    }
}

[[nodiscard]] FORCE_INLINE float3
random_unit_vector(std::uint32_t &state) noexcept
{
    for (;;)
    {
        const float3 v {random(state) * 2.0f - 1.0f,
                      random(state) * 2.0f - 1.0f,
                      random(state) * 2.0f - 1.0f};
        if (const auto length_sq {dot(v, v)};
            length_sq > 0.0f && length_sq <= 1.0f)
        {
            return v * (1.0f / std::sqrt(length_sq));
        }
    }
}

#endif // RANDOM_HPP
