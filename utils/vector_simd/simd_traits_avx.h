
#pragma once

#include "simd_traits.h"
#include <cstdlib>

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

