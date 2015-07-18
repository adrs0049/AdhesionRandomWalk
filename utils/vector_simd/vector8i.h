
#pragma once

#include "vector_base.h"
#include "simd_traits.h"
#include <immintrin.h>

class vector8i : public simd_vector<vector8i>
{
    using value_type = typename simd_vector_traits<vector8i>::value_type;

    public:
        inline vector8i() {}
        inline vector8i(value_type i) : m_value(_mm256_set1_epi32(i)) {}
        inline vector8i(value_type i0, value_type i1,
				        value_type i2, value_type i3,
						value_type i4, value_type i5,
						value_type i6, value_type i7)
			: m_value(_mm256_setr_epi32(i0, i1, i2, i3,
										i4, i5, i6, i7))
		{}

		inline vector8i(const __m256i& rhs) : m_value(rhs) {}

        inline vector8i& operator=(const __m256i& rhs)
        {
            m_value = rhs;
            return *this;
        }

        inline operator __m256i() const { return m_value; }

        inline vector8i& load_a(const value_type* src)
        {
            m_value = _mm256_load_si256((__m256i const*)src);
            return *this;
        }

        inline vector8i& load_u(const value_type* src)
        {
            m_value = _mm256_loadu_si256((__m256i const*)src);
            return *this;
        }

		/*
		inline vector8i& load_partial(int n, const value_type* src)
		{
			switch(n)
			{
				case 0:
					*this = 0; break;
				case 1:
					m_value = _mm_cvtsi32_si128(*src); break;
				case 2:
					m_value = _mm_setr_epi32(((int32_t*)src)[0],
							((int32_t*)src)[1], 0, 0); break;
				case 3:
					m_value = _mm256_setr_epi32(((int32_t*)src)[0],
							((int32_t*)src)[1], ((int32_t*)src)[2], 0); break;
				case 4:
					load_u(src); break;
				default:
					break;
			};
			return *this;
		}
		*/

        inline void store_a(value_type* dst) const
        {
            _mm256_store_si256((__m256i*)dst, m_value);
        }

        inline void store_u(value_type* dst) const
        {
            _mm256_storeu_si256((__m256i*)dst, m_value);
        }

		/*
		inline void store_partial(int n, value_type* dst) const
		{
			union {
				int32_t i[4];
				int64_t q[2];
			} u;

			switch(n)
			{
				case 1:
					*(int32_t*)dst = _mm_cvtsi128_si32(m_value);
					break;
				case 2:
					store_u(u.i);
					*(int64_t*)dst = u.q[0];
					break;
				case 3:
					store_u(u.i);
					*(int64_t*)dst		= u.q[0];
					((int32_t*)dst)[2]	= u.i[2];
					break;
				case 4:
					store_u(dst);
					break;
				default:
					break;
			};
		}
		*/
    private:
        __m256i m_value;
};

class vector8ib : public simd_vector_bool<vector8ib>
{
	using value_type = int32_t;

	public:
        inline vector8ib() {}
        inline vector8ib(bool b)
			: m_value(_mm256_set1_epi32(-(int)b)) {}
        inline vector8ib(bool b0, bool b1, bool b2, bool b3,
						 bool b4, bool b5, bool b6, bool b7)
            : m_value(_mm256_setr_epi32(-(value_type)b0,-(value_type)b1,
										-(value_type)b2,-(value_type)b3,
										-(value_type)b4,-(value_type)b5,
										-(value_type)b6,-(value_type)b7))
		{}

        inline vector8ib(const __m256i& rhs) : m_value(rhs) {}

        inline vector8ib& operator=(const __m256i& rhs)
        {
            m_value = rhs;
            return *this;
        }

		inline vector8ib& operator=(bool b)
		{
			*this = vector8ib(b);
			return *this;
		}

		inline vector8ib& load_a(const value_type* src)
        {
            m_value = _mm256_load_si256((__m256i const*)src);
            return *this;
        }

        inline vector8ib& load_u(const value_type* src)
        {
            m_value = _mm256_loadu_si256((__m256i const*)src);
            return *this;
        }

		/*
		inline vector8ib& load_partial(int n, const value_type* src)
		{
			switch(n)
			{
				case 0:
					*this = false; break;
				case 1:
					m_value = _mm_cvtsi32_si128(*src); break;
				case 2:
					m_value = _mm_setr_epi32(((int32_t*)src)[0],
							((int32_t*)src)[1], 0, 0); break;
				case 3:
					m_value = _mm_setr_epi32(((int32_t*)src)[0],
							((int32_t*)src)[1], ((int32_t*)src)[2], 0); break;
				case 4:
					load_u(src); break;
				default:
					break;
			};
			return *this;
		}
		*/

        inline void store_a(value_type* dst) const
        {
            _mm256_store_si256((__m256i*)dst, m_value);
        }

        inline void store_u(value_type* dst) const
        {
            _mm256_storeu_si256((__m256i*)dst, m_value);
        }

		/*
		inline void store_partial(int n, value_type* dst) const
		{
			union {
				int32_t i[4];
				int64_t q[2];
			} u;

			switch(n)
			{
				case 1:
					*(int32_t*)dst = _mm_cvtsi128_si32(m_value);
					break;
				case 2:
					store_u(u.i);
					*(int64_t*)dst = u.q[0];
					break;
				case 3:
					store_u(u.i);
					*(int64_t*)dst		= u.q[0];
					((int32_t*)dst)[2]	= u.i[2];
					break;
				case 4:
					store_u(dst);
					break;
				default:
					break;
			};
		}
		*/

		inline vector8ib(int) = delete;
		inline vector8ib& operator=(int) = delete;

        inline operator __m256i() const { return m_value; }

    private:
        __m256i m_value;
};

// Operations
inline vector8i select(const vector8ib& cond, const vector8i& a, const vector8i& b)
{
#if SSE_INSTT_SET >= 5
    return _mm256_blendv_epi8(b, a, cond);
#else
    return _mm256_or_si256(_mm256_and_si256(cond, a), _mm256_andnot_si256(cond, b));
#endif
}

inline vector8i operator+(const vector8i& lhs, const vector8i& rhs)
{
    return _mm256_add_epi32(lhs,rhs);
}

inline vector8i operator-(const vector8i& lhs, const vector8i& rhs)
{
    return _mm256_sub_epi32(lhs,rhs);
}

// unary minus
inline vector8i operator-(const vector8i& rhs)
{
	return _mm256_sub_epi32(_mm256_setzero_si256(), rhs);
}

inline vector8i operator*(const vector8i& lhs, const vector8i& rhs)
{
    return _mm256_mullo_epi32(lhs,rhs);
}

inline vector8ib operator&(const vector8i& lhs, const vector8i& rhs)
{
    return _mm256_and_si256(lhs,rhs);
}

inline vector8ib operator|(const vector8i& lhs, const vector8i& rhs)
{
    return _mm256_or_si256(lhs,rhs);
}

inline vector8ib operator^(const vector8i& lhs, const vector8i& rhs)
{
    return _mm256_xor_si256(lhs,rhs);
}

inline vector8i operator~(const vector8i& rhs)
{
	return vector8i(_mm256_xor_si256(rhs, _mm256_set1_epi32(-1)));
}

inline vector8ib operator&(const vector8ib& lhs, const vector8ib& rhs)
{
    return _mm256_and_si256(lhs,rhs);
}

inline vector8ib operator|(const vector8ib& lhs, const vector8ib& rhs)
{
    return _mm256_or_si256(lhs,rhs);
}

inline vector8ib operator^(const vector8ib& lhs, const vector8ib& rhs)
{
    return _mm256_xor_si256(lhs,rhs);
}

inline vector8ib operator~(const vector8ib& rhs)
{
    return _mm256_xor_si256(rhs,_mm256_set1_epi32(-1));
}

inline vector8ib operator==(const vector8i& lhs, const vector8i& rhs)
{
    return _mm256_cmpeq_epi32(lhs,rhs);
}

// comparisons for vector8i
inline vector8ib operator!=(const vector8i& lhs, const vector8i& rhs)
{
	return vector8ib(vector8i (~(lhs == rhs)));
}

inline vector8ib operator==(const vector8ib& lhs, const vector8ib& rhs)
{
    return _mm256_cmpeq_epi32(lhs,rhs);
}

inline vector8ib operator!=(const vector8ib& lhs, const vector8ib& rhs)
{
	return vector8ib(vector8i (~(lhs == rhs)));
}

inline vector8ib operator>(const vector8i& lhs, const vector8i& rhs)
{
	return _mm256_cmpgt_epi32(lhs, rhs);
}

inline vector8ib operator<(const vector8i& lhs, const vector8i& rhs)
{
	return rhs > lhs;
}

inline vector8ib operator>=(const vector8i& lhs, const vector8i& rhs)
{
	return vector8ib(vector8i (~(rhs > lhs)));
}

inline vector8ib operator<=(const vector8i& lhs, const vector8i& rhs)
{
	return rhs >= lhs;
}

