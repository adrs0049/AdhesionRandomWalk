
#pragma once

#include "vector_base.h"
#include "simd_traits.h"
#include <immintrin.h>

class vector256b : public simd_vector<vector256b>
{
	using value_type = bool;

	public:
		inline vector256b() {}
		inline vector256b(const __m256i& rhs) : m_value(rhs) {}

        inline vector256b& operator=(const __m256i& rhs)
        {
            m_value = rhs;
            return *this;
        }

        inline operator __m256i() const { return m_value; }

        inline vector256b& load_a(const void* src)
        {
            m_value = _mm256_load_si256((__m256i const*)src);
            return *this;
        }

        inline vector256b& load_u(const void* src)
        {
            m_value = _mm256_loadu_si256((__m256i const*)src);
            return *this;
        }

        inline void store_a(void* dst) const
        {
            _mm256_store_si256((__m256i*)dst, m_value);
        }

        inline void store_u(void* dst) const
        {
            _mm256_storeu_si256((__m256i*)dst, m_value);
        }

		vector256b const & set_bit(uint32_t index, int value)
		{
			static uint64_t m[8] = {0,0,0,0,1,0,0,0};
			int wi = (index >> 6) & 3;
			int bi = index & 0x3F;

			__m256i mask = vector256b().load(m+4-wi);
			mask = _mm256_sll_epi64(mask, _mm_cvtsi32_si128(bi));
			if (value & 1)
			{
				m_value = _mm256_or_si256(mask,m_value);
			}
			else
			{
				m_value = _mm256_andnot_si256(mask,m_value);
			}
			return *this;
		}

		int get_bit(uint32_t index) const
		{
			union {
				__m256i x;
				uint8_t i[32];
			} u;
			u.x = m_value;
			int wi = (index >> 3) & 0x1F;
			int bi = index & 7;
			return (u.[wi] >> bi) & 1;
		}

		bool operator [] (int32_t index) const
		{
			return get_bit(index) != 0;
		}

    private:
        __m256i m_value;
};

inline vector256b operator&(const vector256b& lhs, const vector256b& rhs)
{
    return _mm256_and_si256(lhs,rhs);
}

inline vector256b operator&&(const vector256b& lhs, const vector256b& rhs)
{
	return lhs & rhs;
}

inline vector256b operator|(const vector256b& lhs, const vector256b& rhs)
{
    return _mm256_or_si256(lhs,rhs);
}

inline vector256b operator||(const vector256b& lhs, const vector256b& rhs)
{
	return lhs | rhs;
}

inline vector256b operator^(const vector256b& lhs, const vector256b& rhs)
{
    return _mm256_xor_si256(lhs,rhs);
}

inline vector256b operator~(const vector256b& rhs)
{
	return _mm256_xor_si256(rhs, _mm256_set1_epi32(-1));
}

inline vector256b operator==(const vector256b& lhs, const vector256b& rhs)
{
    return _mm256_cmpeq_epi32(lhs,rhs);
}

// comparisons for vector8i
inline vector256b operator!=(const vector256b& lhs, const vector256b& rhs)
{
	return vector256b(~(lhs == rhs));
}

inline vector256b andnot(const vector256b& lhs, const vector256b& rhs)
{
	return _mm256_andnot_si256(rhs,lhs);
}

class vector32i : public simd_vector<vector32i>
{
	public:
		vector32i() {}
		vector32i(int i) : m_value(_mm256_set1_epi8((char)i)) {}

		vector32i& load_u(const void* src)
		{
			m_value = _mm256_loadu_si256((__m256 const*)src);
			return *this;
		}

		vector32i& load_a(const void* src)
		{
			m_value = _mm256_load_si256((__m256 const*)src);
			return *this;
		}

		vector32i& load_partial(int n, const void* src)
		{
		}

	private:
		__m256i m_value;
};

inline vector32i operator + (const vector32i& lhs, const vector32i& rhs)
{
	return _mm256_add_epi8(lhs, rhs);
}

inline vector32i operator - (const vector32i& lhs, const vector32i& rhs)
{
	return _mm256_sub_epi8(lhs, rhs);
}

inline vector32i operator - (const vector32i& rhs)
{
	return _mm256_sub_epi8(_mm256_setzero_si256(), rhs);
}


class vector32ib : public simd_vector_bool<vector32ib>
{};

inline vector32ib operator & (const vector32ib& lhs, const vector32ib& rhs)
{
	return vector32ib(vector256b(lhs) & vector256b(rhs));
}

// trial
class vector8i : public vector256b
{
	public:
		inline vector8i() {}
		inline vector8i(const __m256i& rhs) : vector256b(rhs) {}

};


