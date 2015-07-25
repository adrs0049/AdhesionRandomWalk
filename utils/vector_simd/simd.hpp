#pragma once

#include "simd_config.h"
#include "simd_traits.h"

#if SSE_INSTR_SET > 6
	#include "simd_traits_avx.h"
#elif SSE_INSTR_SET > 0
	#include "simd_traits_see.h"
#endif

#include "invoker.h"

#include <vector2d.h>
#include <vector4f.h>
#include <vector4d.h>
#include <vector8f.h>

// int vectors
#include <vector4i.h>
#include <vector8i.h>


// TODO MOVE these functions somewhere else

static inline vector4d to_double(const vector4i& a)
{
    return _mm256_cvtepi32_pd(a);
}

