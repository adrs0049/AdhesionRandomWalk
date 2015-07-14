
#pragma once

#include "vector_base.h"
#include "simd_traits.h"

#include <emmintrin.h>

class vector2d : public simd_vector<vector2d>
{
    using value_type = typename simd_vector_traits<vector2d>::value_type;

    public:
        inline vector2d() {}
        inline vector2d(double f) : m_value(_mm_set1_pd(f)) {}
        inline vector2d(double d0, double d1)
			: m_value(_mm_setr_pd(d0, d1)) {}
        inline vector2d(const __m128d& rhs) : m_value(rhs) {}

        inline vector2d& operator=(const __m128d& rhs)
        {
            m_value = rhs;
            return *this;
        }

        inline operator __m128d() const { return m_value; }

        inline vector2d& load_a(const value_type* src)
        {
            m_value = _mm_load_pd(src);
            return *this;
        }

        inline vector2d& load_u(const value_type* src)
        {
            m_value = _mm_loadu_pd(src);
            return *this;
        }

        inline void store_a(value_type* dst) const
        {
            _mm_store_pd(dst, m_value);
        }

        inline void store_u(value_type* dst) const
        {
            _mm_storeu_pd(dst, m_value);
        }

    private:
        __m128d m_value;

};

class vector2db : public simd_vector_bool<vector2db>
{
    public:
        inline vector2db() {}
        inline vector2db(bool b)
			: m_value(_mm_castsi128_pd(_mm_set1_epi32(-(int)b))) {}
        inline vector2db(bool b0, bool b1)
            : m_value(_mm_castsi128_pd(
						_mm_setr_epi32(-(int)b0,-(int)b0,
							           -(int)b1,-(int)b1)))
		{}

        inline vector2db(const __m128d& rhs) : m_value(rhs) {}

        inline vector2db& operator=(const __m128d& rhs)
        {
            m_value = rhs;
            return *this;
        }

        inline operator __m128d() const { return m_value; }

    private:
        __m128d m_value;
};

// Operations
vector2d select(const vector2db& cond, const vector2d& a, const vector2d& b)
{
#if SSE_INSTT_SET >= 5
    return _mm_blendv_pd(b, a, cond);
#else
    return _mm_or_pd(_mm_and_pd(cond, a), _mm_andnot_pd(cond, b));
#endif
}

inline vector2d operator+(const vector2d& lhs, const vector2d& rhs)
{
    return _mm_add_pd(lhs,rhs);
}

inline vector2d operator-(const vector2d& lhs, const vector2d& rhs)
{
    return _mm_sub_pd(lhs,rhs);
}

inline vector2d operator*(const vector2d& lhs, const vector2d& rhs)
{
    return _mm_mul_pd(lhs,rhs);
}

inline vector2d operator/(const vector2d& lhs, const vector2d& rhs)
{
    return _mm_div_pd(lhs,rhs);
}

inline vector2db operator&(const vector2d& lhs, const vector2d& rhs)
{
    return _mm_and_pd(lhs,rhs);
}

inline vector2db operator|(const vector2d& lhs, const vector2d& rhs)
{
    return _mm_or_pd(lhs,rhs);
}

inline vector2db operator^(const vector2d& lhs, const vector2d& rhs)
{
    return _mm_xor_pd(lhs,rhs);
}

/*
inline vector2db operator~(const vector2d& rhs)
{
    return _mm_xor_pd(rhs,_mm_castsi128_ps(_mm_set1_epi32(-1)));
}
*/

inline vector2db operator&(const vector2db& lhs, const vector2db& rhs)
{
    return _mm_and_pd(lhs,rhs);
}

inline vector2db operator|(const vector2db& lhs, const vector2db& rhs)
{
    return _mm_or_pd(lhs,rhs);
}

inline vector2db operator^(const vector2db& lhs, const vector2db& rhs)
{
    return _mm_xor_pd(lhs,rhs);
}

inline vector2db operator~(const vector2db& rhs)
{
    return _mm_xor_pd(rhs,_mm_castsi128_pd(_mm_set1_epi32(-1)));
}

inline vector2db operator==(const vector2db& lhs, const vector2db& rhs)
{
    return _mm_cmpeq_pd(lhs,rhs);
}

// comparisons for vector2d
inline vector2db operator!=(const vector2d& lhs, const vector2db& rhs)
{
    return _mm_cmpneq_pd(lhs,rhs);
}

inline vector2db operator==(const vector2d& lhs, const vector2d& rhs)
{
        return _mm_cmpeq_pd(lhs,rhs);
}

inline vector2db operator!=(const vector2d& lhs, const vector2d& rhs)
{
        return _mm_cmpneq_pd(lhs,rhs);
}

inline vector2db operator<(const vector2d& lhs, const vector2d& rhs)
{
        return _mm_cmplt_pd(lhs,rhs);
}

inline vector2db operator<=(const vector2d& lhs, const vector2d& rhs)
{
        return _mm_cmple_pd(lhs,rhs);
}
