
#pragma once

#include "vector_base.h"
#include "simd_traits.h"

#include <immintrin.h>

template<int i0, int i1, int i2, int i3, int i4, int i5, int i6, int i7>
static inline __m256 constant8f()
{
	static const union
	{
		int32_t		i[8];
		__m256		ymm;
	} u = {{i0, i1, i2, i3, i4, i5, i6, i7}};
	return u.ymm;
}

class vector8f : public simd_vector<vector8f>
{
    using value_type = typename simd_vector_traits<vector8f>::value_type;

    public:
        inline vector8f() {}
        inline vector8f(float f) : m_value(_mm256_set1_ps(f)) {}
        inline vector8f(float f0, float f1, float f2, float f3,
					    float f4, float f5, float f6, float f7)
			: m_value(_mm256_setr_ps(f0, f1, f2, f3, f4, f5, f6, f7))
		{}

		inline vector8f(const __m256& rhs) : m_value(rhs) {}

        inline vector8f& operator=(const __m256& rhs)
        {
            m_value = rhs;
            return *this;
        }

        inline operator __m256() const { return m_value; }

        inline vector8f& load_a(const value_type* src)
        {
            m_value = _mm256_load_ps(src);
            return *this;
        }

        inline vector8f& load_u(const value_type* src)
        {
            m_value = _mm256_loadu_ps(src);
            return *this;
        }

        inline void store_a(value_type* dst) const
        {
            _mm256_store_ps(dst, m_value);
        }

        inline void store_u(value_type* dst) const
        {
            _mm256_storeu_ps(dst, m_value);
        }

    private:
        __m256 m_value;

};

class vector8fb : public simd_vector_bool<vector8fb>
{
    public:
        inline vector8fb() {}
        inline vector8fb(bool b)
			: m_value(_mm256_castsi256_ps(_mm256_set1_epi32(-(int)b))) {}

		inline vector8fb(bool b0, bool b1, bool b2, bool b3,
						 bool b4, bool b5, bool b6, bool b7)
            : m_value(_mm256_castsi256_ps(
				_mm256_setr_epi32(-(int)b0,-(int)b1,-(int)b2,-(int)b3,
								  -(int)b4,-(int)b5,-(int)b6,-(int)b7)))
	    {}

        inline vector8fb(const __m256& rhs) : m_value(rhs) {}

        inline vector8fb& operator=(const __m256& rhs)
        {
            m_value = rhs;
            return *this;
        }

        inline operator __m256() const { return m_value; }

    private:
        __m256 m_value;
};

// Operations
inline vector8f select(const vector8fb& cond, const vector8f& a, const vector8f& b)
{
	return _mm256_blendv_ps(b, a, cond);
}

inline vector8f operator+(const vector8f& lhs, const vector8f& rhs)
{
	return _mm256_add_ps(lhs,rhs);
}

inline vector8f operator-(const vector8f& lhs, const vector8f& rhs)
{
	return _mm256_sub_ps(lhs,rhs);
}

inline vector8f operator*(const vector8f& lhs, const vector8f& rhs)
{
	return _mm256_mul_ps(lhs,rhs);
}

inline vector8f operator/(const vector8f& lhs, const vector8f& rhs)
{
	return _mm256_div_ps(lhs,rhs);
}

// c[i] = a[i] * b[i] + c[i]
inline vector8f mul_add(const vector8f& a, const vector8f& b, const vector8f& c)
{
	return _mm256_fmadd_ps(a, b, c);
}

// c[i] = a[i] * b[i] - c[i]
inline vector8f mul_sub(const vector8f& a, const vector8f& b, const vector8f& c)
{
	return _mm256_fmsub_ps(a, b, c);
}

// c[i] = - (a[i] * b[i]) + c[i]
inline vector8f nmul_add(const vector8f& a, const vector8f& b, const vector8f& c)
{
	return _mm256_fnmadd_ps(a, b, c);
}

// c[i] = - (a[i] * b[i]) - c[i]
inline vector8f nmul_sub(const vector8f& a, const vector8f& b, const vector8f& c)
{
	return _mm256_fnmsub_ps(a, b, c);
}

inline vector8f max(const vector8f& a, const vector8f& b)
{
	return _mm256_max_ps(a, b);
}

inline vector8f min(const vector8f& a, const vector8f& b)
{
	return _mm256_min_ps(a, b);
}

inline vector8f abs(const vector8f& a)
{
	__m256 mask = constant8f<0x7FFFFFFF,0x7FFFFFFF,0x7FFFFFFF,0x7FFFFFFF,0x7FFFFFFF,0x7FFFFFFF,0x7FFFFFFF,0x7FFFFFFF> ();
	return _mm256_and_ps(a, mask);
}

inline vector8f sqrt(const vector8f& a)
{
	return _mm256_sqrt_ps(a);
}

inline vector8f square(const vector8f& a)
{
	return a * a;
}

inline vector8f rcp(const vector8f& a)
{
	return _mm256_rcp_ps(a);
}

inline vector8f rsqrt(const vector8f& a)
{
	return _mm256_rsqrt_ps(a);
}

inline float hadd(const vector8f& a)
{
	__m256 t1 = _mm256_hadd_ps(a, a);
	__m256 t2 = _mm256_hadd_ps(t1, t1);
	__m128 t3 = _mm256_extractf128_ps(t2, 1);
	__m128 t4 = _mm_add_ss(_mm256_castps256_ps128(t2), t3);
	return _mm_cvtss_f32(t4);
}

inline vector8fb operator&(const vector8f& lhs, const vector8f& rhs)
{
	return _mm256_and_ps(lhs,rhs);
}

inline vector8fb operator|(const vector8f& lhs, const vector8f& rhs)
{
	return _mm256_or_ps(lhs,rhs);
}

inline vector8fb operator^(const vector8f& lhs, const vector8f& rhs)
{
	return _mm256_xor_ps(lhs,rhs);
}

inline vector8fb operator~(const vector8f& rhs)
{
	return _mm256_xor_ps(rhs,_mm256_castsi256_ps(_mm256_set1_epi32(-1)));
}

inline vector8fb operator&(const vector8fb& lhs, const vector8fb& rhs)
{
	return _mm256_and_ps(lhs,rhs);
}

inline vector8fb operator|(const vector8fb& lhs, const vector8fb& rhs)
{
	return _mm256_or_ps(lhs,rhs);
}

inline vector8fb operator^(const vector8fb& lhs, const vector8fb& rhs)
{
	return _mm256_xor_ps(lhs,rhs);
}

inline vector8fb operator~(const vector8fb& rhs)
{
    return _mm256_xor_ps(rhs,_mm256_castsi256_ps(_mm256_set1_epi32(-1)));
}

inline vector8fb operator==(const vector8fb& lhs, const vector8fb& rhs)
{
	return _mm256_cmp_ps(lhs,rhs,_CMP_EQ_UQ);
}

// comparisons for vector8f
inline vector8fb operator!=(const vector8f& lhs, const vector8fb& rhs)
{
	return _mm256_cmp_ps(lhs,rhs,_CMP_NEQ_UQ);
}

inline vector8fb operator==(const vector8f& lhs, const vector8f& rhs)
{
	return _mm256_cmp_ps(lhs,rhs,_CMP_EQ_UQ);
}

inline vector8fb operator!=(const vector8f& lhs, const vector8f& rhs)
{
	return _mm256_cmp_ps(lhs,rhs,_CMP_NEQ_UQ);
}

inline vector8fb operator<(const vector8f& lhs, const vector8f& rhs)
{
	return _mm256_cmp_ps(lhs,rhs,_CMP_LE_OQ);
}

inline vector8fb operator<=(const vector8f& lhs, const vector8f& rhs)
{
	return _mm256_cmp_ps(lhs,rhs,_CMP_LE_OQ);
}
