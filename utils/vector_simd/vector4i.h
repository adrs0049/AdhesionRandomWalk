
#pragma once

#include "vector_base.h"
#include "simd_traits.h"
#include <pmmintrin.h>

class vector4i : public simd_vector<vector4i>
{
    using value_type = typename simd_vector_traits<vector4i>::value_type;

    public:
        inline vector4i() {}
        inline vector4i(value_type i) : m_value(_mm_set1_epi32(i)) {}
        inline vector4i(value_type i0, value_type i1,
				        value_type i2, value_type i3)
			: m_value(_mm_setr_epi32(i0, i1, i2, i3))
		{}

		inline vector4i(const __m128i& rhs) : m_value(rhs) {}

        inline vector4i& operator=(const __m128i& rhs)
        {
            m_value = rhs;
            return *this;
        }

        inline operator __m128i() const { return m_value; }

        inline vector4i& load_a(const value_type* src)
        {
            m_value = _mm_load_si128((__m128i const*)src);
            return *this;
        }

        inline vector4i& load_u(const value_type* src)
        {
            m_value = _mm_loadu_si128((__m128i const*)src);
            return *this;
        }

		inline vector4i& load_partial(int n, const value_type* src)
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
					m_value = _mm_setr_epi32(((int32_t*)src)[0],
							((int32_t*)src)[1], ((int32_t*)src)[2], 0); break;
				case 4:
					load_u(src); break;
				default:
					break;
			};
			return *this;
		}

        inline void store_a(value_type* dst) const
        {
            _mm_store_si128((__m128i*)dst, m_value);
        }

        inline void store_u(value_type* dst) const
        {
            _mm_storeu_si128((__m128i*)dst, m_value);
        }

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

    private:
        __m128i m_value;
};

class vector4ib : public simd_vector_bool<vector4ib>
{
	using value_type = int32_t;

	public:
        inline vector4ib() {}
        inline vector4ib(bool b)
			: m_value(_mm_set1_epi32(-(int)b)) {}
        inline vector4ib(bool b0, bool b1, bool b2, bool b3)
            : m_value(_mm_setr_epi32(-(value_type)b0,-(value_type)b1,
								     -(value_type)b2,-(value_type)b3))
		{}

        inline vector4ib(const __m128i& rhs) : m_value(rhs) {}

        inline vector4ib& operator=(const __m128i& rhs)
        {
            m_value = rhs;
            return *this;
        }

		inline vector4ib& operator=(bool b)
		{
			*this = vector4ib(b);
			return *this;
		}

		inline vector4ib& load_a(const value_type* src)
        {
            m_value = _mm_load_si128((__m128i const*)src);
            return *this;
        }

        inline vector4ib& load_u(const value_type* src)
        {
            m_value = _mm_loadu_si128((__m128i const*)src);
            return *this;
        }

		inline vector4ib& load_partial(int n, const value_type* src)
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

        inline void store_a(value_type* dst) const
        {
            _mm_store_si128((__m128i*)dst, m_value);
        }

        inline void store_u(value_type* dst) const
        {
            _mm_storeu_si128((__m128i*)dst, m_value);
        }

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

		inline vector4ib(int) = delete;
		inline vector4ib& operator=(int) = delete;

        inline operator __m128i() const { return m_value; }

    private:
        __m128i m_value;
};

// Operations
inline vector4i select(const vector4ib& cond, const vector4i& a, const vector4i& b)
{
#if SSE_INSTT_SET >= 5
    return _mm_blendv_epi8(b, a, cond);
#else
    return _mm_or_si128(_mm_and_si128(cond, a), _mm_andnot_si128(cond, b));
#endif
}

inline vector4i operator+(const vector4i& lhs, const vector4i& rhs)
{
    return _mm_add_epi32(lhs,rhs);
}

inline vector4i operator-(const vector4i& lhs, const vector4i& rhs)
{
    return _mm_sub_epi32(lhs,rhs);
}

// unary minus
inline vector4i operator-(const vector4i& rhs)
{
	return _mm_sub_epi32(_mm_setzero_si128(), rhs);
}

inline vector4i operator*(const vector4i& lhs, const vector4i& rhs)
{
    return _mm_mullo_epi32(lhs,rhs);
}

inline vector4i operator/(const vector4i& lhs, const vector4i& rhs)
{
    return _mm_div_epi32(lhs,rhs);
}

inline vector4ib operator&(const vector4i& lhs, const vector4i& rhs)
{
    return _mm_and_epi32(lhs,rhs);
}

inline vector4ib operator|(const vector4i& lhs, const vector4i& rhs)
{
    return _mm_or_epi32(lhs,rhs);
}

inline vector4ib operator^(const vector4i& lhs, const vector4i& rhs)
{
    return _mm_xor_epi32(lhs,rhs);
}

inline vector4i operator~(const vector4i& rhs)
{
	return vector4i(_mm_xor_si128(rhs, _mm_set1_epi32(-1)));
}

inline vector4ib operator&(const vector4ib& lhs, const vector4ib& rhs)
{
    return _mm_and_epi32(lhs,rhs);
}

inline vector4ib operator|(const vector4ib& lhs, const vector4ib& rhs)
{
    return _mm_or_epi32(lhs,rhs);
}

inline vector4ib operator^(const vector4ib& lhs, const vector4ib& rhs)
{
    return _mm_xor_epi32(lhs,rhs);
}

inline vector4ib operator~(const vector4ib& rhs)
{
    return _mm_xor_epi32(rhs,_mm_set1_epi32(-1));
}

inline vector4ib operator==(const vector4ib& lhs, const vector4ib& rhs)
{
    return _mm_cmpeq_epi32(lhs,rhs);
}

// comparisons for vector4i
inline vector4ib operator!=(const vector4i& lhs, const vector4ib& rhs)
{
	return vector4ib(vector4i (~(lhs == rhs)));
}

inline vector4ib operator==(const vector4i& lhs, const vector4i& rhs)
{
    return _mm_cmpeq_epi32(lhs,rhs);
}

inline vector4ib operator!=(const vector4i& lhs, const vector4i& rhs)
{
	return vector4ib(vector4i (~(lhs == rhs)));
}

inline vector4ib operator>(const vector4i& lhs, const vector4i& rhs)
{
	return _mm_cmpgt_epi32(lhs, rhs);
}

inline vector4ib operator<(const vector4i& lhs, const vector4i& rhs)
{
	return rhs > lhs;
}

inline vector4ib operator>=(const vector4i& lhs, const vector4i& rhs)
{
	return vector4ib(vector4i (~(rhs > lhs)));
}

inline vector4ib operator<=(const vector4i& lhs, const vector4i& rhs)
{
	return rhs >= lhs;
}

