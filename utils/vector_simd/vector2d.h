
#pragma once

#include "vector_base.h"
#include "simd_traits.h"

#include <immintrin.h>
#include <pmmintrin.h>

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

		inline vector2d& load_partial(int n, const value_type* src)
		{
			if (n == 1)
			{
				m_value = _mm_load_sd(src);
			}
			else if (n == 2)
			{
				load_u(src);
			}
			else
			{
				m_value = _mm_setzero_pd();
			}
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

		inline void store_partial(int n, value_type* dst) const
		{
			if (n == 1)
			{
				_mm_store_sd(dst, m_value);
			}
			else if (n == 2)
			{
				store_u(dst);
			}
		}

		double extract(uint32_t index) const
		{
			double x[2];
			store_u(x);
			return x[index & 1];
		}

		double operator [] (uint32_t index) const
		{
			return extract(index);
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
/* FIXME why doesn't this work anymore??
vector2d select(const vector2db& cond, const vector2d& a, const vector2d& b)
{
#if SSE_INSTT_SET >= 5
    return _mm_blendv_pd(b, a, cond);
#else
    return _mm_or_pd(_mm_and_pd(cond, a), _mm_andnot_pd(cond, b));
#endif
}
*/

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

// c[i] = a[i] * b[i] + c[i]
inline vector2d mul_add(const vector2d& a, const vector2d& b, const vector2d& c)
{
	return _mm_fmadd_pd(a, b, c);
}

// c[i] = a[i] * b[i] - c[i]
inline vector2d mul_sub(const vector2d& a, const vector2d& b, const vector2d& c)
{
	return _mm_fmsub_pd(a, b, c);
}

// c[i] = - (a[i] * b[i]) + c[i]
inline vector2d nmul_add(const vector2d& a, const vector2d& b, const vector2d& c)
{
	return _mm_fnmadd_pd(a, b, c);
}

// c[i] = - (a[i] * b[i]) - c[i]
inline vector2d nmul_sub(const vector2d& a, const vector2d& b, const vector2d& c)
{
	return _mm_fnmsub_pd(a, b, c);
}

inline vector2d max(const vector2d& a, const vector2d& b)
{
	return _mm_max_pd(a, b);
}

inline vector2d min(const vector2d& a, const vector2d& b)
{
	return _mm_min_pd(a, b);
}

inline vector2d abs(const vector2d& a)
{
	__m128d mask = _mm_castsi128_pd(
			_mm_setr_epi32(-1, 0x7FFFFFFF, -1, 0x7FFFFFFF));
	return _mm_and_pd(a, mask);
}

inline vector2d sqrt(const vector2d& a)
{
	return _mm_sqrt_pd(a);
}

inline vector2d square(const vector2d& a)
{
	return a * a;
}

inline double hadd(const vector2d& a)
{
	__m128d t1 = _mm_hadd_pd(a, a);
	return _mm_cvtsd_f64(t1);
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
