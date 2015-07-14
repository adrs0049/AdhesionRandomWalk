
#pragma once

#include "vector_base.h"
#include "simd_traits.h"

#include <immintrin.h>

template<int i0, int i1, int i2, int i3, int i4, int i5, int i6, int i7>
static inline __m256 constant8f()
{
	static const union
	{
		int		i[8];
		__m256	ymm;
	} u = {{i0,i1,i2,i3,i4,i5,i6,i7}};
	return u.ymm;
}

class vector4d : public simd_vector<vector4d>
{
    using value_type = typename simd_vector_traits<vector4d>::value_type;

    public:
        inline vector4d() {}
        inline vector4d(double d) : m_value(_mm256_set1_pd(d)) {}
        inline vector4d(double d0, double d1, double d2, double d3)
			: m_value(_mm256_setr_pd(d0, d1, d2, d3)) {}

        inline vector4d(const __m256d& rhs)
			: m_value(rhs) {}

        inline vector4d& operator=(const __m256d& rhs)
        {
            m_value = rhs;
            return *this;
        }

        inline operator __m256d() const { return m_value; }

        inline vector4d& load_a(const value_type* src)
        {
            m_value = _mm256_load_pd(src);
            return *this;
        }

        inline vector4d& load_u(const value_type* src)
        {
            m_value = _mm256_loadu_pd(src);
            return *this;
        }

        inline void store_a(value_type* dst) const
        {
            _mm256_store_pd(dst, m_value);
        }

        inline void store_u(value_type* dst) const
        {
            _mm256_storeu_pd(dst, m_value);
        }

    private:
        __m256d m_value;

};

class vector4db : public simd_vector_bool<vector4db>
{
    public:
        inline vector4db() {}
        inline vector4db(bool b)
			: m_value(_mm256_castsi256_pd(_mm256_set1_epi32(-(int)b))) {}
        inline vector4db(bool b0, bool b1, bool b2, bool b3)
            : m_value(_mm256_castsi256_pd(_mm256_setr_epi64x(-(int)b0,
							-(int)b1,-(int)b2,-(int)b3))) {}

        inline vector4db(const __m256d& rhs) : m_value(rhs) {}

        inline vector4db& operator=(const __m256d& rhs)
        {
            m_value = rhs;
            return *this;
        }

        inline operator __m256d() const { return m_value; }

    private:
        __m256d m_value;
};

// Operations
inline vector4d select(const vector4db& cond, const vector4d& a, const vector4d& b)
{
	return _mm256_blendv_pd(b, a, cond);
}

inline vector4d operator+(const vector4d& lhs, const vector4d& rhs)
{
	return _mm256_add_pd(lhs,rhs);
}

inline vector4d operator-(const vector4d& lhs, const vector4d& rhs)
{
	return _mm256_sub_pd(lhs,rhs);
}

inline vector4d operator*(const vector4d& lhs, const vector4d& rhs)
{
	return _mm256_mul_pd(lhs,rhs);
}

inline vector4d operator/(const vector4d& lhs, const vector4d& rhs)
{
	return _mm256_div_pd(lhs,rhs);
}

inline vector4db operator&(const vector4d& lhs, const vector4d& rhs)
{
	return _mm256_and_pd(lhs,rhs);
}

inline vector4db operator|(const vector4d& lhs, const vector4d& rhs)
{
	return _mm256_or_pd(lhs,rhs);
}

inline vector4db operator^(const vector4d& lhs, const vector4d& rhs)
{
	return _mm256_xor_pd(lhs,rhs);
}

/*
inline vector4db operator~(const vector4d& rhs)
{
	return _mm256_xor_pd(rhs, _mm256_casti256_pd(_mm256_set1_epi32(-1)));
}
*/

inline vector4db operator&(const vector4db& lhs, const vector4db& rhs)
{
	return _mm256_and_pd(lhs,rhs);
}

inline vector4db operator|(const vector4db& lhs, const vector4db& rhs)
{
	return _mm256_or_pd(lhs,rhs);
}

inline vector4db operator^(const vector4db& lhs, const vector4db& rhs)
{
	return _mm256_xor_pd(lhs,rhs);
}

inline vector4db operator~(const vector4db& rhs)
{
	return _mm256_xor_pd(rhs, _mm256_castps_pd(
				constant8f<-1,-1,-1,-1,-1,-1,-1,-1>()));
}

inline vector4db operator==(const vector4db& lhs, const vector4db& rhs)
{
	return _mm256_cmp_pd(lhs,rhs,_CMP_EQ_UQ);
}

// comparisons for vector4d
inline vector4db operator!=(const vector4d& lhs, const vector4db& rhs)
{
	return _mm256_cmp_pd(lhs,rhs,_CMP_NEQ_UQ);
}

inline vector4db operator==(const vector4d& lhs, const vector4d& rhs)
{
	return _mm256_cmp_pd(lhs,rhs,_CMP_EQ_UQ);
}

inline vector4db operator!=(const vector4d& lhs, const vector4d& rhs)
{
	return _mm256_cmp_pd(lhs,rhs,_CMP_NEQ_UQ);
}

inline vector4db operator<(const vector4d& lhs, const vector4d& rhs)
{
	return _mm256_cmp_pd(lhs,rhs,_CMP_LT_OQ);
}

inline vector4db operator<=(const vector4d& lhs, const vector4d& rhs)
{
	return _mm256_cmp_pd(lhs,rhs,_CMP_LE_OQ);
}
