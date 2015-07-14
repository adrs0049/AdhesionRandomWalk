
#pragma once

#include "simd_traits.h"
#include <cstdlib>

template<>
struct simd_traits<int>
{
    using type = vector4i;
    static const std::size_t size = 4;
};

template<>
struct simd_traits<float>
{
    using type = vector4f;
    static const std::size_t size = 4;
};

template<>
struct simd_traits<double>
{
    using type = vector2d;
    static const std::size_t size = 2;
};



