
#pragma once

#include <cstdint>

template<typename T> struct simd_vector_traits;

class vector256b;

template<>
struct simd_vector_traits<vector256b>
{
	using value_type = bool;
	using vector_bool = vector256b;
};

class vector4i;
class vector4ib;

template<>
struct simd_vector_traits<vector4i>
{
    using value_type = int32_t;
    using vector_bool = vector4ib;
};

class vector8i;
class vector8ib;

template<>
struct simd_vector_traits<vector8i>
{
	using value_type = int32_t;
	using vector_bool = vector8ib;
};

class vector4f;
class vector4fb;

template<>
struct simd_vector_traits<vector4f>
{
    using value_type = float;
    using vector_bool = vector4fb;
};

class vector2d;
class vector2db;

template<>
struct simd_vector_traits<vector2d>
{
	using value_type = double;
	using vector_bool = vector2db;
};

class vector8f;
class vector8fb;

template<>
struct simd_vector_traits<vector8f>
{
	using value_type = float;
	using vector_bool = vector8fb;
};

class vector4d;
class vector4db;

template<>
struct simd_vector_traits<vector4d>
{
	using value_type = double;
	using vector_bool = vector4db;
};

template<class T>
struct simd_traits
{
    using value = T;
    static const std::size_t size = 1;
};

