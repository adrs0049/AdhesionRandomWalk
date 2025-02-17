// test
#include <cxxtest/TestSuite.h>

#include <iostream>
#include <memory>
#include <iterator>
#include <vector>
#include <algorithm>
#include "debug.h"
#include <cassert>

#include <iterator>
#include <random>

#include "Terminate.h"
#include <invoker.h>
#include <simd_traits.h>
#include "simd_traits_see.h"
#include "vector4i.h"
#include <AlignmentAllocator.h>

#include "vector_ops.h"

class Vector4iTest : public CxxTest::TestSuite
{
private:
    double tol = 1E-5;

	using vector_type = std::vector<int,
		  AlignedAllocator<int, Alignment::SSE>>;

	using vector_bool_type = std::vector<bool,
		  AlignedAllocator<bool, Alignment::SSE>>;

public:

    void setUp(void)
    {
		Error::TerminalCatcher::init();
    }

    void tearDown(void)
    {}

	void testBasic(void)
	{
		vector4i test;
	}

	void testAdditionConst(void)
	{
		vector_type a, b, c;
		unsigned int length = 20;

		a.resize(length, 1.0);
		b.resize(length, 1.0);

		TS_ASSERT_EQUALS(a.size(), b.size());

		auto result = a + b;

		std::cerr << "Testing SIMD instructions" << std::endl;

		using vec_type = simd_traits<int>::type;
		size_t vec_size = simd_traits<int>::size;

		std::size_t n = a.size();
		c.resize(length, 0.0);

		//std::cerr << "vec_size="<<vec_size<<
		//	" n="<<n<<" n/vec="<<n/vec_size<<std::endl;
		for(size_t i = 0; i < n; i += vec_size)
		{
			//std::cerr<<"i="<<i<<std::endl;
			vec_type av = load_a(&a[i]);
			vec_type bv = load_a(&b[i]);

			//std::cerr << "vec_a=("<<av[0] <<", "<<av[1]<<", "<<av[2]
			//			<<", "<<av[3]<<")" << std::endl;

			//std::cerr << "vec_b=("<<bv[0] <<", "<<bv[1]<<", "<<bv[2]
			//			<<", "<<bv[3]<<")" << std::endl;

			vec_type cv = av + bv;

			//std::cerr << "vec_c=("<<cv[0] <<", "<<cv[1]<<", "<<cv[2]
			//			<<", "<<cv[3]<<")" << std::endl;

			store_a(&c[i],cv);
			//std::cerr << "vec_c_std=("<<c[i]<<", "<<c[i+1]<<", "<<c[i+2]
			//		  <<", "<<c[i+3]<<")"<<std::endl;
		}


		for (std::size_t i = 0; i < length; i++)
		{
			//std::cout<<"result["<<i<<"]="<<result[i]
			//	<<", c["<<i<<"]="<<c[i]<<std::endl;
			TS_ASSERT_DELTA(result[i], c[i], tol);
		}
	}

	void testSubstractionConst(void)
	{
		vector_type a, b, c;
		unsigned int length = 20;

		a.resize(length, 2.0);
		b.resize(length, 1.0);

		TS_ASSERT_EQUALS(a.size(), b.size());

		auto result = a - b;

		std::cerr << "Testing SIMD instructions" << std::endl;

		using vec_type = simd_traits<int>::type;
		size_t vec_size = simd_traits<int>::size;

		std::size_t n = a.size();
		c.resize(length, 0.0);

		//std::cerr << "vec_size="<<vec_size<<
		//	" n="<<n<<" n/vec="<<n/vec_size<<std::endl;
		for(size_t i = 0; i < n; i += vec_size)
		{
			//std::cerr<<"i="<<i<<std::endl;
			vec_type av = load_a(&a[i]);
			vec_type bv = load_a(&b[i]);

			//std::cerr << "vec_a=("<<av[0] <<", "<<av[1]<<", "<<av[2]
			//			<<", "<<av[3]<<")" << std::endl;

			//std::cerr << "vec_b=("<<bv[0] <<", "<<bv[1]<<", "<<bv[2]
			//			<<", "<<bv[3]<<")" << std::endl;

			vec_type cv = av - bv;

			//std::cerr << "vec_c=("<<cv[0] <<", "<<cv[1]<<", "<<cv[2]
			//			<<", "<<cv[3]<<")" << std::endl;

			store_a(&c[i],cv);
			//std::cerr << "vec_c_std=("<<c[i]<<", "<<c[i+1]<<", "<<c[i+2]
			//		  <<", "<<c[i+3]<<")"<<std::endl;
		}


		for (std::size_t i = 0; i < length; i++)
		{
			//std::cout<<"result["<<i<<"]="<<result[i]
			//	<<", c["<<i<<"]="<<c[i]<<std::endl;
			TS_ASSERT_DELTA(result[i], c[i], tol);
		}
	}

	void testMultiplicationConst(void)
	{
		vector_type a, b, c;
		unsigned int length = 20;

		a.resize(length, 2.0);
		b.resize(length, 2.0);

		TS_ASSERT_EQUALS(a.size(), b.size());

		auto result = a * b;

		std::cerr << "Testing SIMD instructions" << std::endl;

		using vec_type = simd_traits<int>::type;
		size_t vec_size = simd_traits<int>::size;

		std::size_t n = a.size();
		c.resize(length, 0.0);

		//std::cerr << "vec_size="<<vec_size<<
		//	" n="<<n<<" n/vec="<<n/vec_size<<std::endl;
		for(size_t i = 0; i < n; i += vec_size)
		{
			//std::cerr<<"i="<<i<<std::endl;
			vec_type av = load_a(&a[i]);
			vec_type bv = load_a(&b[i]);

			//std::cerr << "vec_a=("<<av[0] <<", "<<av[1]<<", "<<av[2]
			//			<<", "<<av[3]<<")" << std::endl;

			//std::cerr << "vec_b=("<<bv[0] <<", "<<bv[1]<<", "<<bv[2]
			//			<<", "<<bv[3]<<")" << std::endl;

			vec_type cv = av * bv;

			//std::cerr << "vec_c=("<<cv[0] <<", "<<cv[1]<<", "<<cv[2]
			//			<<", "<<cv[3]<<")" << std::endl;

			store_a(&c[i],cv);
			//std::cerr << "vec_c_std=("<<c[i]<<", "<<c[i+1]<<", "<<c[i+2]
			//		  <<", "<<c[i+3]<<")"<<std::endl;
		}


		for (std::size_t i = 0; i < length; i++)
		{
			//std::cout<<"result["<<i<<"]="<<result[i]
			//	<<", c["<<i<<"]="<<c[i]<<std::endl;
			TS_ASSERT_DELTA(result[i], c[i], tol);
		}
	}

	void testInkrementConst(void)
	{
		vector_type a, b, c, result;
		unsigned int length = 20;

		a.resize(length, 4.0);
		b.resize(length, 0.0);
		c.resize(length, 0.0);
		result.resize(length, 5.0);

		using vec_type = simd_traits<int>::type;
		size_t vec_size = simd_traits<int>::size;

		std::size_t n = a.size();

		//std::cerr << "vec_size="<<vec_size<<
		//	" n="<<n<<" n/vec="<<n/vec_size<<std::endl;
		for(size_t i = 0; i < n; i += vec_size)
		{
			//std::cerr<<"i="<<i<<std::endl;
			vec_type av = load_a(&a[i]);
			vec_type bv = load_a(&a[i]);

			//std::cerr << "vec_a=("<<av[0] <<", "<<av[1]<<", "<<av[2]
			//			<<", "<<av[3]<<")" << std::endl;

			av++;
			++bv;

			//std::cerr << "vec_b=("<<bv[0] <<", "<<bv[1]<<", "<<bv[2]
			//			<<", "<<bv[3]<<")" << std::endl;

			//std::cerr << "vec_a=("<<av[0] <<", "<<av[1]<<", "<<av[2]
			//			<<", "<<av[3]<<")" << std::endl;

			store_a(&b[i],av);
			store_a(&c[i],bv);
		}

		for (std::size_t i = 0; i < length; i++)
		{
			TS_ASSERT_DELTA(result[i], c[i], tol);
			TS_ASSERT_DELTA(result[i], b[i], tol);
		}
	}

	void testDekrementConst(void)
	{
		vector_type a, b, c, result;
		unsigned int length = 20;

		a.resize(length, 4.0);
		b.resize(length, 0.0);
		c.resize(length, 0.0);
		result.resize(length, 3.0);

		using vec_type = simd_traits<int>::type;
		size_t vec_size = simd_traits<int>::size;

		std::size_t n = a.size();

		//std::cerr << "vec_size="<<vec_size<<
		//	" n="<<n<<" n/vec="<<n/vec_size<<std::endl;
		for(size_t i = 0; i < n; i += vec_size)
		{
			//std::cerr<<"i="<<i<<std::endl;
			vec_type av = load_a(&a[i]);
			vec_type bv = load_a(&a[i]);

			//std::cerr << "vec_a=("<<av[0] <<", "<<av[1]<<", "<<av[2]
			//			<<", "<<av[3]<<")" << std::endl;

			av--;
			--bv;

			//std::cerr << "vec_b=("<<bv[0] <<", "<<bv[1]<<", "<<bv[2]
			//			<<", "<<bv[3]<<")" << std::endl;

			//std::cerr << "vec_a=("<<av[0] <<", "<<av[1]<<", "<<av[2]
			//			<<", "<<av[3]<<")" << std::endl;

			store_a(&b[i],av);
			store_a(&c[i],bv);
		}

		for (std::size_t i = 0; i < length; i++)
		{
			TS_ASSERT_DELTA(result[i], c[i], tol);
			TS_ASSERT_DELTA(result[i], b[i], tol);
		}
	}

	void testAddition(void)
	{
		unsigned int length = 200;
		int runs = 1000;

		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<> dis(0,2);

		for (int run = 0; run < runs; run++)
		{
			vector_type a, b, c;

			for (std::size_t i = 0; i < length; i++)
			{
				a.push_back(dis(gen));
				b.push_back(dis(gen));
			}

			TS_ASSERT_EQUALS(a.size(), b.size());

			auto result = a + b;

			using vec_type = simd_traits<int>::type;
			size_t vec_size = simd_traits<int>::size;

			std::size_t n = a.size();

			c.resize(length, 0.0);

			TS_ASSERT_EQUALS(a.size(), n);
			TS_ASSERT_EQUALS(b.size(), n);

			for(size_t i = 0; i < n; i += vec_size)
			{
				vec_type av = load_a(&a[i]);
				vec_type bv = load_a(&b[i]);

				vec_type cv = av + bv;
				store_a(&c[i],cv);
			}

			TS_ASSERT_EQUALS(a.size(), b.size());
			TS_ASSERT_EQUALS(a.size(), c.size());
			TS_ASSERT_EQUALS(b.size(), c.size());

			for (std::size_t i = 0; i < length; i++)
			{
				TS_ASSERT_DELTA(result[i], c[i], tol);
			}

		}
	}

	void testSubstraction(void)
	{
		unsigned int length = 200;
		int runs = 1000;

		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<> dis(0,2);

		for (int run = 0; run < runs; run++)
		{
			vector_type a, b, c;

			for (std::size_t i = 0; i < length; i++)
			{
				a.push_back(dis(gen));
				b.push_back(dis(gen));
			}

			TS_ASSERT_EQUALS(a.size(), b.size());

			auto result = a - b;

			using vec_type = simd_traits<int>::type;
			size_t vec_size = simd_traits<int>::size;

			std::size_t n = a.size();

			c.resize(length, 0.0);

			TS_ASSERT_EQUALS(a.size(), n);
			TS_ASSERT_EQUALS(b.size(), n);

			for(size_t i = 0; i < n; i += vec_size)
			{
				vec_type av = load_a(&a[i]);
				vec_type bv = load_a(&b[i]);

				vec_type cv = av - bv;
				store_a(&c[i],cv);
			}

			TS_ASSERT_EQUALS(a.size(), b.size());
			TS_ASSERT_EQUALS(a.size(), c.size());
			TS_ASSERT_EQUALS(b.size(), c.size());

			for (std::size_t i = 0; i < length; i++)
			{
				TS_ASSERT_DELTA(result[i], c[i], tol);
			}
		}
	}

	void testMultiplication(void)
	{
		unsigned int length = 200;
		int runs = 1000;

		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<> dis(0,2);

		for (int run = 0; run < runs; run++)
		{
			vector_type a, b, c;

			for (std::size_t i = 0; i < length; i++)
			{
				a.push_back(dis(gen));
				b.push_back(dis(gen));
			}

			TS_ASSERT_EQUALS(a.size(), b.size());

			auto result = a * b;

			using vec_type = simd_traits<int>::type;
			size_t vec_size = simd_traits<int>::size;

			std::size_t n = a.size();

			c.resize(length, 0.0);

			TS_ASSERT_EQUALS(a.size(), n);
			TS_ASSERT_EQUALS(b.size(), n);

			for(size_t i = 0; i < n; i += vec_size)
			{
				vec_type av = load_a(&a[i]);
				vec_type bv = load_a(&b[i]);

				vec_type cv = av * bv;
				store_a(&c[i],cv);
			}

			TS_ASSERT_EQUALS(a.size(), b.size());
			TS_ASSERT_EQUALS(a.size(), c.size());
			TS_ASSERT_EQUALS(b.size(), c.size());

			for (std::size_t i = 0; i < length; i++)
			{
				TS_ASSERT_DELTA(result[i], c[i], tol);
			}
		}
	}

	void testInkrement(void)
	{
		unsigned int length = 200;
		int runs = 1000;

		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<> dis(0,2);

		for (int run = 0; run < runs; run++)
		{
			vector_type a, b, c, d, result;

			for(std::size_t i = 0; i < length; i++)
			{
				a.push_back(dis(gen));
			}

			b.resize(length, 0.0);
			c.resize(length, 0.0);
			d.resize(length, 1.0);

			result = a + d;

			using vec_type = simd_traits<int>::type;
			size_t vec_size = simd_traits<int>::size;

			std::size_t n = a.size();

			for(size_t i = 0; i < n; i += vec_size)
			{
				vec_type av = load_a(&a[i]);
				vec_type bv = load_a(&a[i]);

				av++;
				++bv;

				store_a(&b[i],av);
				store_a(&c[i],bv);
			}

			for (std::size_t i = 0; i < length; i++)
			{
				TS_ASSERT_DELTA(result[i], c[i], tol);
				TS_ASSERT_DELTA(result[i], b[i], tol);
			}

		}
	}

	void testDekrement(void)
	{
		unsigned int length = 200;
		int runs = 1000;

		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<> dis(0,2);

		for (int run = 0; run < runs; run++)
		{
			vector_type a, b, c, d, result;

			for(std::size_t i = 0; i < length; i++)
			{
				a.push_back(dis(gen));
			}

			b.resize(length, 0.0);
			c.resize(length, 0.0);
			d.resize(length, 1.0);

			result = a - d;

			using vec_type = simd_traits<int>::type;
			size_t vec_size = simd_traits<int>::size;

			std::size_t n = a.size();

			for(size_t i = 0; i < n; i += vec_size)
			{
				vec_type av = load_a(&a[i]);
				vec_type bv = load_a(&a[i]);

				av--;
				--bv;

				store_a(&b[i],av);
				store_a(&c[i],bv);
			}

			for (std::size_t i = 0; i < length; i++)
			{
				TS_ASSERT_DELTA(result[i], c[i], tol);
				TS_ASSERT_DELTA(result[i], b[i], tol);
			}

		}
	}
};

