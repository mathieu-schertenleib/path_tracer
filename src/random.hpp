#ifndef RANDOM_HPP
#define RANDOM_HPP

#include "definitions.hpp"
#include "vec.hpp"

#include <limits>

[[nodiscard]] FORCE_INLINE constexpr u32 seed(u32 x) noexcept
{
    x += x << 10;
    x ^= x >> 6;
    x += x << 3;
    x ^= x >> 11;
    x += x << 15;
    return x;
}

[[nodiscard]] FORCE_INLINE constexpr float random(u32 &rng_state) noexcept
{
    rng_state ^= rng_state << 13;
    rng_state ^= rng_state >> 17;
    rng_state ^= rng_state << 5;
    return static_cast<float>(rng_state) *
           (1.0f / static_cast<float>(std::numeric_limits<u32>::max()));
}

// Returns a random f32v3 with length <= 1
[[nodiscard]] FORCE_INLINE constexpr f32v3 random_in_sphere(u32 &state) noexcept
{
    for (;;)
    {
        const f32v3 v {random(state) * 2.0f - 1.0f,
                        random(state) * 2.0f - 1.0f,
                        random(state) * 2.0f - 1.0f};
        if (vec::dot(v, v) <= 1.0f)
        {
            return v;
        }
    }
}

[[nodiscard]] FORCE_INLINE f32v3 random_unit_vector(u32 &rng_state) noexcept
{
    for (;;)
    {
        const f32v3 v {random(rng_state) * 2.0f - 1.0f,
                        random(rng_state) * 2.0f - 1.0f,
                        random(rng_state) * 2.0f - 1.0f};
        if (const auto length_sq {vec::dot(v, v)};
            length_sq > 1e-8f && length_sq <= 1.0f)
        {
            return v * (1.0f / math::sqrt(length_sq));
        }
    }
}

#endif // RANDOM_HPP
