#ifndef DEFINITIONS_HPP
#define DEFINITIONS_HPP

#ifdef _MSC_VER
#define FORCE_INLINE __forceinline inline
#else
#define FORCE_INLINE __attribute__((always_inline)) inline
#endif

#include <cstdint>

using u32 = std::uint32_t;

#endif // DEFINITIONS_HPP
