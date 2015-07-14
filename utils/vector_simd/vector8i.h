
#pragma once

#include "vector_base.h"
#include "simd_traits.h"
#include <nmmintrin.h>
#include <emmintrin.h>

class vector8i : public simd_vector<vector8i>
{
    using value_type = typename simd_vector_traits<vector8i>::value_type;

    public:
        inline vector8i() {}
        inline vector8i(float f) : m_value(_mm_set1_ps(f)) {}
        inline vector8i(float f0, float f1, float f2, float f3)
			: m_value(_mm_setr_ps(f0, f1, f2, f3)) {}
        inline vector8i(const __m128& rhs) : m_value(rhs) {}

        inline vector8i& operator=(const __m128& rhs)
        {
            m_value = rhs;
            return *this;
        }

        inline operator __m128() const { return m_value; }

        inline vector8i& load_a(const value_type* src)
        {
            m_value = _mm_load_ps(src);
            return *this;
        }

        inline vector8i& load_u(const value_type* src)
        {
            m_value = _mm_loadu_ps(src);
            return *this;
        }

        inline void store_a(value_type* dst) const
        {
            _mm_store_ps(dst, m_value);
        }

        inline void store_u(value_type* dst) const
        {
            _mm_storeu_ps(dst, m_value);
        }

    private:
        __m128 m_value;

};

class vector8ib : public simd_vector_bool<vector8ib>
{
		using value_type = bool;

	public:
        inline vector8ib() {}
        inline vector8ib(bool b)
			: m_value(_mm_castsi128_ps(_mm_set1_epi32(-(int)b))) {}
        inline vector8ib(bool b0, bool b1, bool b2, bool b3)
            : m_value(_mm_castsi128_ps(_mm_setr_epi32(-(int)b0,-(int)b1,
							-(int)b2,-(int)b3))) {}

        inline vector8ib(const __m128& rhs) : m_value(rhs) {}

        inline vector8ib& operator=(const __m128& rhs)
        {
            m_value = rhs;
            return *this;
        }

		inline vector8ib& operator=(bool b)
		{
			*this = vector8ib(b);
			return *this;
		}

		inline vector8ib(int) = delete;
		inline vector8ib& operator=(int) = delete;

        inline operator __m128() const { return m_value; }

    private:
        __m128 m_value;
};

// Operations
inline vector8i select(const vector8ib& cond, const vector8i& a, const vector8i& b)
{
#if SSE_INSTT_SET >= 5
    return _mm_blendv_ps(b, a, cond);
#else
    return _mm_or_ps(_mm_and_ps(cond, a), _mm_andnot_ps(cond, b));
#endif
}

inline vector8i operator+(const vector8i& lhs, const vector8i& rhs)
{
    return _mm_add_ps(lhs,rhs);
}

inline vector8i operator-(const vector8i& lhs, const vector8i& rhs)
{
    return _mm_sub_ps(lhs,rhs);
}

inline vector8i operator*(const vector8i& lhs, const vector8i& rhs)
{
    return _mm_mul_ps(lhs,rhs);
}

inline vector8i operator/(const vector8i& lhs, const vector8i& rhs)
{
    return _mm_div_ps(lhs,rhs);
}

inline vector8ib operator&(const vector8i& lhs, const vector8i& rhs)
{
    return _mm_and_ps(lhs,rhs);
}

inline vector8ib operator|(const vector8i& lhs, const vector8i& rhs)
{
    return _mm_or_ps(lhs,rhs);
}

inline vector8ib operator^(const vector8i& lhs, const vector8i& rhs)
{
    return _mm_xor_ps(lhs,rhs);
}

inline vector8ib operator~(const vector8i& rhs)
{
    return _mm_xor_ps(rhs,_mm_castsi128_ps(_mm_set1_epi32(-1)));
}

inline vector8ib operator&(const vector8ib& lhs, const vector8ib& rhs)
{
    return _mm_and_ps(lhs,rhs);
}

inline vector8ib operator|(const vector8ib& lhs, const vector8ib& rhs)
{
    return _mm_or_ps(lhs,rhs);
}

inline vector8ib operator^(const vector8ib& lhs, const vector8ib& rhs)
{
    return _mm_xor_ps(lhs,rhs);
}

inline vector8ib operator~(const vector8ib& rhs)
{
    return _mm_xor_ps(rhs,_mm_castsi128_ps(_mm_set1_epi32(-1)));
}

inline vector8ib operator==(const vector8ib& lhs, const vector8ib& rhs)
{
    return _mm_cmpeq_ps(lhs,rhs);
}

// comparisons for vector8i
inline vector8ib operator!=(const vector8i& lhs, const vector8ib& rhs)
{
    return _mm_cmpneq_ps(lhs,rhs);
}

inline vector8ib operator==(const vector8i& lhs, const vector8i& rhs)
{
        return _mm_cmpeq_ps(lhs,rhs);
}

inline vector8ib operator!=(const vector8i& lhs, const vector8i& rhs)
{
        return _mm_cmpneq_ps(lhs,rhs);
}

inline vector8ib operator<(const vector8i& lhs, const vector8i& rhs)
{
        return _mm_cmplt_ps(lhs,rhs);
}

inline vector8ib operator<=(const vector8i& lhs, const vector8i& rhs)
{
        return _mm_cmple_ps(lhs,rhs);
}

