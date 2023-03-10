#ifndef SIMD_HPP
#define SIMD_HPP

#include "definitions.hpp"

#include <immintrin.h>

namespace simd
{

// FIXME: this only supports AVX for now
struct vfloat
{
    __m256 v;
};

[[nodiscard]] FORCE_INLINE vfloat zero()
{
    return {_mm256_setzero_ps()};
}

[[nodiscard]] FORCE_INLINE vfloat broadcast(float a)
{
    return {_mm256_set1_ps(a)};
}

[[nodiscard]] FORCE_INLINE vfloat
set(float a, float b, float c, float d, float e, float f, float g, float h)
{
    return {_mm256_set_ps(a, b, c, d, e, f, g, h)};
}

[[nodiscard]] FORCE_INLINE vfloat broadcast(const float *p)
{
    return {_mm256_broadcast_ss(p)};
}

// Aligned
[[nodiscard]] FORCE_INLINE vfloat load(const float *p)
{
    return {_mm256_load_ps(p)};
}

// Aligned
FORCE_INLINE void store(float *p, vfloat a)
{
    _mm256_store_ps(p, a.v);
}

// Unaligned
[[nodiscard]] FORCE_INLINE vfloat loadu(const float *p)
{
    return {_mm256_loadu_ps(p)};
}

// Unaligned
FORCE_INLINE void storeu(float *p, vfloat a)
{
    _mm256_storeu_ps(p, a.v);
}

[[nodiscard]] FORCE_INLINE vfloat operator+(vfloat a)
{
    return a;
}

[[nodiscard]] FORCE_INLINE vfloat operator-(vfloat a)
{
    return {_mm256_sub_ps(_mm256_setzero_ps(), a.v)};
}

[[nodiscard]] FORCE_INLINE vfloat operator+(vfloat a, vfloat b)
{
    return {_mm256_add_ps(a.v, b.v)};
}

[[nodiscard]] FORCE_INLINE vfloat operator-(vfloat a, vfloat b)
{
    return {_mm256_sub_ps(a.v, b.v)};
}

[[nodiscard]] FORCE_INLINE vfloat operator*(vfloat a, vfloat b)
{
    return {_mm256_mul_ps(a.v, b.v)};
}

[[nodiscard]] FORCE_INLINE vfloat operator/(vfloat a, vfloat b)
{
    return {_mm256_div_ps(a.v, b.v)};
}

FORCE_INLINE vfloat &operator+=(vfloat &a, vfloat b)
{
    a = a + b;
    return a;
}

FORCE_INLINE vfloat &operator-=(vfloat &a, vfloat b)
{
    a = a - b;
    return a;
}

FORCE_INLINE vfloat &operator*=(vfloat &a, vfloat b)
{
    a = a * b;
    return a;
}

FORCE_INLINE vfloat &operator/=(vfloat &a, vfloat b)
{
    a = a / b;
    return a;
}

[[nodiscard]] FORCE_INLINE vfloat min(vfloat a, vfloat b)
{
    return {_mm256_min_ps(a.v, b.v)};
}

[[nodiscard]] FORCE_INLINE vfloat max(vfloat a, vfloat b)
{
    return {_mm256_max_ps(a.v, b.v)};
}

[[nodiscard]] FORCE_INLINE vfloat abs(vfloat a)
{
    return {
        _mm256_and_ps(a.v, _mm256_castsi256_ps(_mm256_set1_epi32(0x7fffffff)))};
}

[[nodiscard]] FORCE_INLINE vfloat sqrt(vfloat a)
{
    return {_mm256_sqrt_ps(a.v)};
}

[[nodiscard]] FORCE_INLINE vfloat rsqrt(vfloat a)
{
    return {_mm256_rsqrt_ps(a.v)};
}

[[nodiscard]] FORCE_INLINE vfloat rcp(vfloat a)
{
    return {_mm256_rcp_ps(a.v)};
}

[[nodiscard]] FORCE_INLINE bool all_positive(vfloat a)
{
    return _mm256_testz_ps(a.v, _mm256_castsi256_ps(_mm256_set1_epi32(-1)));
}

[[nodiscard]] FORCE_INLINE bool all_negative(vfloat a)
{
    return _mm256_testc_ps(a.v, _mm256_castsi256_ps(_mm256_set1_epi32(-1)));
}

[[nodiscard]] FORCE_INLINE vfloat round(vfloat a)
{
    return {_mm256_round_ps(a.v, _MM_FROUND_NINT)};
}

[[nodiscard]] FORCE_INLINE vfloat floor(vfloat a)
{
    return {_mm256_round_ps(a.v, _MM_FROUND_FLOOR)};
}

[[nodiscard]] FORCE_INLINE vfloat ceil(vfloat a)
{
    return {_mm256_round_ps(a.v, _MM_FROUND_CEIL)};
}

[[nodiscard]] FORCE_INLINE vfloat trunc(vfloat a)
{
    return {_mm256_round_ps(a.v, _MM_FROUND_TRUNC)};
}

[[nodiscard]] FORCE_INLINE vfloat fmadd(vfloat a, vfloat b, vfloat c)
{
    return {_mm256_fmadd_ps(a.v, b.v, c.v)};
}

[[nodiscard]] FORCE_INLINE vfloat fmsub(vfloat a, vfloat b, vfloat c)
{
    return {_mm256_fmsub_ps(a.v, b.v, c.v)};
}

struct mask
{
    __m256 v;
};

[[nodiscard]] FORCE_INLINE mask operator==(vfloat a, vfloat b)
{
    return {_mm256_cmp_ps(a.v, b.v, _CMP_EQ_OQ)};
}

[[nodiscard]] FORCE_INLINE mask operator!=(vfloat a, vfloat b)
{
    return {_mm256_cmp_ps(a.v, b.v, _CMP_NEQ_OQ)};
}

[[nodiscard]] FORCE_INLINE mask operator<(vfloat a, vfloat b)
{
    return {_mm256_cmp_ps(a.v, b.v, _CMP_LT_OQ)};
}

[[nodiscard]] FORCE_INLINE mask operator<=(vfloat a, vfloat b)
{
    return {_mm256_cmp_ps(a.v, b.v, _CMP_LE_OQ)};
}

[[nodiscard]] FORCE_INLINE mask operator>(vfloat a, vfloat b)
{
    return {_mm256_cmp_ps(a.v, b.v, _CMP_GT_OQ)};
}

[[nodiscard]] FORCE_INLINE mask operator>=(vfloat a, vfloat b)
{
    return {_mm256_cmp_ps(a.v, b.v, _CMP_GE_OQ)};
}

[[nodiscard]] FORCE_INLINE vfloat masked(vfloat a, mask m)
{
    return {_mm256_and_ps(a.v, m.v)};
}

[[nodiscard]] FORCE_INLINE vfloat select(vfloat a, vfloat b, mask m)
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

[[nodiscard]] FORCE_INLINE bool none(mask m)
{
    return _mm256_testz_ps(m.v, _mm256_castsi256_ps(_mm256_set1_epi32(-1)));
}

[[nodiscard]] FORCE_INLINE bool all(mask m)
{
    return _mm256_testc_ps(m.v, _mm256_castsi256_ps(_mm256_set1_epi32(-1)));
}

} // namespace simd

#endif // SIMD_HPP
