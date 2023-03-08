#ifndef SIMD_HPP
#define SIMD_HPP

#include "definitions.hpp"

#include <immintrin.h>

// FIXME: this only supports AVX for now
struct vf32
{
    __m256 v;

    [[nodiscard]] static FORCE_INLINE vf32 zero()
    {
        return vf32 {_mm256_setzero_ps()};
    }

    [[nodiscard]] static FORCE_INLINE vf32 broadcast(float a)
    {
        return vf32 {_mm256_set1_ps(a)};
    }

    [[nodiscard]] static FORCE_INLINE vf32
    set(float a, float b, float c, float d, float e, float f, float g, float h)
    {
        return vf32 {_mm256_set_ps(a, b, c, d, e, f, g, h)};
    }

    [[nodiscard]] static FORCE_INLINE vf32 broadcast(const float *p)
    {
        return vf32 {_mm256_broadcast_ss(p)};
    }

    // Aligned
    [[nodiscard]] static FORCE_INLINE vf32 load(const float *p)
    {
        return vf32 {_mm256_load_ps(p)};
    }

    // Aligned
    static FORCE_INLINE void store(float *p, vf32 a)
    {
        _mm256_store_ps(p, a.v);
    }

    // Unaligned
    [[nodiscard]] static FORCE_INLINE vf32 loadu(const float *p)
    {
        return vf32 {_mm256_loadu_ps(p)};
    }

    // Unaligned
    static FORCE_INLINE void storeu(float *p, vf32 a)
    {
        _mm256_storeu_ps(p, a.v);
    }
};

[[nodiscard]] FORCE_INLINE vf32 operator+(vf32 a)
{
    return a;
}

[[nodiscard]] FORCE_INLINE vf32 operator-(vf32 a)
{
    return vf32 {_mm256_sub_ps(_mm256_setzero_ps(), a.v)};
}

[[nodiscard]] FORCE_INLINE vf32 operator+(vf32 a, vf32 b)
{
    return vf32 {_mm256_add_ps(a.v, b.v)};
}

[[nodiscard]] FORCE_INLINE vf32 operator-(vf32 a, vf32 b)
{
    return vf32 {_mm256_sub_ps(a.v, b.v)};
}

[[nodiscard]] FORCE_INLINE vf32 operator*(vf32 a, vf32 b)
{
    return vf32 {_mm256_mul_ps(a.v, b.v)};
}

[[nodiscard]] FORCE_INLINE vf32 operator/(vf32 a, vf32 b)
{
    return vf32 {_mm256_div_ps(a.v, b.v)};
}

[[nodiscard]] FORCE_INLINE vf32 min(vf32 a, vf32 b)
{
    return vf32 {_mm256_min_ps(a.v, b.v)};
}

[[nodiscard]] FORCE_INLINE vf32 max(vf32 a, vf32 b)
{
    return vf32 {_mm256_max_ps(a.v, b.v)};
}

[[nodiscard]] FORCE_INLINE vf32 sqrt(vf32 a)
{
    return vf32 {_mm256_sqrt_ps(a.v)};
}

[[nodiscard]] FORCE_INLINE vf32 rsqrt(vf32 a)
{
    return vf32 {_mm256_rsqrt_ps(a.v)};
}

[[nodiscard]] FORCE_INLINE vf32 rcp(vf32 a)
{
    return vf32 {_mm256_rcp_ps(a.v)};
}

[[nodiscard]] FORCE_INLINE bool all_positive(vf32 a)
{
    return _mm256_testz_ps(a.v, _mm256_castsi256_ps(_mm256_set1_epi32(-1)));
}

[[nodiscard]] FORCE_INLINE bool all_negative(vf32 a)
{
    return _mm256_testc_ps(a.v, _mm256_castsi256_ps(_mm256_set1_epi32(-1)));
}

struct mask
{
    __m256 v;
};

[[nodiscard]] FORCE_INLINE mask operator==(vf32 a, vf32 b)
{
    return {_mm256_cmp_ps(a.v, b.v, _CMP_EQ_OQ)};
}

[[nodiscard]] FORCE_INLINE mask operator!=(vf32 a, vf32 b)
{
    return {_mm256_cmp_ps(a.v, b.v, _CMP_NEQ_OQ)};
}

[[nodiscard]] FORCE_INLINE mask operator<(vf32 a, vf32 b)
{
    return {_mm256_cmp_ps(a.v, b.v, _CMP_LT_OQ)};
}

[[nodiscard]] FORCE_INLINE mask operator<=(vf32 a, vf32 b)
{
    return {_mm256_cmp_ps(a.v, b.v, _CMP_LE_OQ)};
}

[[nodiscard]] FORCE_INLINE mask operator>(vf32 a, vf32 b)
{
    return {_mm256_cmp_ps(a.v, b.v, _CMP_GT_OQ)};
}

[[nodiscard]] FORCE_INLINE mask operator>=(vf32 a, vf32 b)
{
    return {_mm256_cmp_ps(a.v, b.v, _CMP_GE_OQ)};
}

[[nodiscard]] FORCE_INLINE vf32 masked(vf32 a, mask m)
{
    return {_mm256_and_ps(a.v, m.v)};
}

[[nodiscard]] FORCE_INLINE vf32 blend(vf32 a, vf32 b, mask m)
{
    return {_mm256_blendv_ps(a.v, b.v, m.v)};
}

[[nodiscard]] FORCE_INLINE mask operator~(mask m)
{
    return {_mm256_xor_ps(m.v, _mm256_castsi256_ps(_mm256_set1_epi32(-1)))};
}

[[nodiscard]] FORCE_INLINE mask operator&(mask a, mask b)
{
    return {_mm256_and_ps(a.v, b.v)};
}

[[nodiscard]] FORCE_INLINE mask operator|(mask a, mask b)
{
    return {_mm256_or_ps(a.v, b.v)};
}

[[nodiscard]] FORCE_INLINE bool none_of(mask m)
{
    return _mm256_testz_ps(m.v, _mm256_castsi256_ps(_mm256_set1_epi32(-1)));
}

[[nodiscard]] FORCE_INLINE bool all_of(mask m)
{
    return _mm256_testc_ps(m.v, _mm256_castsi256_ps(_mm256_set1_epi32(-1)));
}

#endif // SIMD_HPP
