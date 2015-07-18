
#pragma once

#include "simd_traits.h"
#include <cstdlib>

template<>
struct simd_traits<bool>
{
	using type = vector256b;
	static const std::size_t size = 256;
};

template<>
struct simd_traits<int32_t>
{
	using type = vector8i;
	static const std::size_t size = 8;
};

template<>
struct simd_traits<float>
{
    using type = vector8f;
    static const std::size_t size = 8;
};

template<>
struct simd_traits<double>
{
    using type = vector4d;
    static const std::size_t size = 4;
};
