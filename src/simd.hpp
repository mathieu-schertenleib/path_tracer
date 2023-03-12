#ifndef SIMD_HPP
#define SIMD_HPP

#include "definitions.hpp"

#include <immintrin.h>

struct vf32
{
    __m256 v;
};

namespace simd
{

[[nodiscard]] FORCE_INLINE vf32 zero()
{
    return {_mm256_setzero_ps()};
}

[[nodiscard]] FORCE_INLINE vf32 broadcast(f32 a)
{
    return {_mm256_set1_ps(a)};
}

[[nodiscard]] FORCE_INLINE vf32
set(f32 a, f32 b, f32 c, f32 d, f32 e, f32 f, f32 g, f32 h)
{
    return {_mm256_set_ps(a, b, c, d, e, f, g, h)};
}

[[nodiscard]] FORCE_INLINE vf32 broadcast(const f32 *p)
{
    return {_mm256_broadcast_ss(p)};
}

[[nodiscard]] FORCE_INLINE vf32 load_aligned(const f32 *p)
{
    return {_mm256_load_ps(p)};
}

FORCE_INLINE void store_aligned(f32 *p, vf32 a)
{
    _mm256_store_ps(p, a.v);
}

[[nodiscard]] FORCE_INLINE vf32 load_unaligned(const f32 *p)
{
    return {_mm256_loadu_ps(p)};
}

FORCE_INLINE void store_unaligned(f32 *p, vf32 a)
{
    _mm256_storeu_ps(p, a.v);
}

[[nodiscard]] FORCE_INLINE vf32 operator+(vf32 a)
{
    return a;
}

[[nodiscard]] FORCE_INLINE vf32 operator-(vf32 a)
{
    return {_mm256_sub_ps(_mm256_setzero_ps(), a.v)};
}

[[nodiscard]] FORCE_INLINE vf32 operator+(vf32 a, vf32 b)
{
    return {_mm256_add_ps(a.v, b.v)};
}

[[nodiscard]] FORCE_INLINE vf32 operator-(vf32 a, vf32 b)
{
    return {_mm256_sub_ps(a.v, b.v)};
}

[[nodiscard]] FORCE_INLINE vf32 operator*(vf32 a, vf32 b)
{
    return {_mm256_mul_ps(a.v, b.v)};
}

[[nodiscard]] FORCE_INLINE vf32 operator/(vf32 a, vf32 b)
{
    return {_mm256_div_ps(a.v, b.v)};
}

FORCE_INLINE vf32 &operator+=(vf32 &a, vf32 b)
{
    a = a + b;
    return a;
}

FORCE_INLINE vf32 &operator-=(vf32 &a, vf32 b)
{
    a = a - b;
    return a;
}

FORCE_INLINE vf32 &operator*=(vf32 &a, vf32 b)
{
    a = a * b;
    return a;
}

FORCE_INLINE vf32 &operator/=(vf32 &a, vf32 b)
{
    a = a / b;
    return a;
}

[[nodiscard]] FORCE_INLINE vf32 min(vf32 a, vf32 b)
{
    return {_mm256_min_ps(a.v, b.v)};
}

[[nodiscard]] FORCE_INLINE vf32 max(vf32 a, vf32 b)
{
    return {_mm256_max_ps(a.v, b.v)};
}

[[nodiscard]] FORCE_INLINE vf32 abs(vf32 a)
{
    return {
        _mm256_and_ps(a.v, _mm256_castsi256_ps(_mm256_set1_epi32(0x7fffffff)))};
}

[[nodiscard]] FORCE_INLINE vf32 sqrt(vf32 a)
{
    return {_mm256_sqrt_ps(a.v)};
}

[[nodiscard]] FORCE_INLINE vf32 rsqrt(vf32 a)
{
    return {_mm256_rsqrt_ps(a.v)};
}

[[nodiscard]] FORCE_INLINE vf32 rcp(vf32 a)
{
    return {_mm256_rcp_ps(a.v)};
}

[[nodiscard]] FORCE_INLINE bool all_positive(vf32 a)
{
    return _mm256_testz_ps(a.v, _mm256_castsi256_ps(_mm256_set1_epi32(-1)));
}

[[nodiscard]] FORCE_INLINE bool all_negative(vf32 a)
{
    return _mm256_testc_ps(a.v, _mm256_castsi256_ps(_mm256_set1_epi32(-1)));
}

[[nodiscard]] FORCE_INLINE vf32 round(vf32 a)
{
    return {_mm256_round_ps(a.v, _MM_FROUND_NINT)};
}

[[nodiscard]] FORCE_INLINE vf32 floor(vf32 a)
{
    return {_mm256_round_ps(a.v, _MM_FROUND_FLOOR)};
}

[[nodiscard]] FORCE_INLINE vf32 ceil(vf32 a)
{
    return {_mm256_round_ps(a.v, _MM_FROUND_CEIL)};
}

[[nodiscard]] FORCE_INLINE vf32 trunc(vf32 a)
{
    return {_mm256_round_ps(a.v, _MM_FROUND_TRUNC)};
}

[[nodiscard]] FORCE_INLINE vf32 fmadd(vf32 a, vf32 b, vf32 c)
{
    return {_mm256_fmadd_ps(a.v, b.v, c.v)};
}

[[nodiscard]] FORCE_INLINE vf32 fmsub(vf32 a, vf32 b, vf32 c)
{
    return {_mm256_fmsub_ps(a.v, b.v, c.v)};
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

[[nodiscard]] FORCE_INLINE vf32 select(vf32 a, vf32 b, mask m)
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
