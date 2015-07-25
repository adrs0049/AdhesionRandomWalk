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
#include <simd_traits.h>
#include <invoker.h>
#include <simd_traits_avx.h>
#include "vector8f.h"
#include <AlignmentAllocator.h>

#include "vector_ops.h"

class Vector8fTest : public CxxTest::TestSuite
{
private:
    double tol = 1E-5;

	using vector_type = std::vector<float,
		  AlignedAllocator<float, Alignment::AVX>>;

	using vector_bool_type = std::vector<bool,
		  AlignedAllocator<bool, Alignment::AVX>>;

public:

    void setUp(void)
    {
		Error::TerminalCatcher::init();
    }

    void tearDown(void)
    {}

	void testBasic(void)
	{
		vector8f test;
	}

	void testAdditionConst(void)
	{
		vector_type a, b, c;
		unsigned int length = 40;

		a.resize(length, 1.0);
		b.resize(length, 1.0);

		TS_ASSERT_EQUALS(a.size(), b.size());

		auto result = a + b;

		std::cerr << "Testing SIMD instructions" << std::endl;

		using vec_type = simd_traits<float>::type;
		size_t vec_size = simd_traits<float>::size;

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

	void testAdditionVar(void)
	{
		unsigned runs = 1000;

		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<> dis(1, 200);

		for (std::size_t run = 0; run < runs; run++)
		{
			// FIXME
			continue;

			vector_type a, b, c;
			unsigned int length = dis(gen);

			std::cerr << "Variable length test with " << length
					  << "." << std::endl;

			a.resize(length, 1.0);
			b.resize(length, 1.0);

			TS_ASSERT_EQUALS(a.size(), b.size());

			auto result = a + b;

			using vec_type = simd_traits<float>::type;
			size_t vec_size = simd_traits<float>::size;

			std::size_t n = a.size();
			c.resize(length, 0.0);

			for(size_t i = 0; i < n; i += vec_size)
			{
				vec_type av = load_a(&a[i]);
				vec_type bv = load_a(&b[i]);

				vec_type cv = av + bv;
				store_a(&c[i],cv);
			}

			for (std::size_t i = 0; i < length; i++)
			{
				//std::cout<<"result["<<i<<"]="<<result[i]
				//	<<", c["<<i<<"]="<<c[i]<<std::endl;
				TS_ASSERT_DELTA(result[i], c[i], tol);
			}
		}
	}

	void testSubstractionConst(void)
	{
		vector_type a, b, c;
		unsigned int length = 40;

		a.resize(length, 2.0);
		b.resize(length, 1.0);

		TS_ASSERT_EQUALS(a.size(), b.size());

		auto result = a - b;

		std::cerr << "Testing SIMD instructions" << std::endl;

		using vec_type = simd_traits<float>::type;
		size_t vec_size = simd_traits<float>::size;

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
		unsigned int length = 40;

		a.resize(length, 2.0);
		b.resize(length, 2.0);

		TS_ASSERT_EQUALS(a.size(), b.size());

		auto result = a * b;

		std::cerr << "Testing SIMD instructions" << std::endl;

		using vec_type = simd_traits<float>::type;
		size_t vec_size = simd_traits<float>::size;

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

	void testDivisionConst(void)
	{
		vector_type a, b, c;
		unsigned int length = 40;

		a.resize(length, 4.0);
		b.resize(length, 2.0);

		TS_ASSERT_EQUALS(a.size(), b.size());

		auto result = a / b;

		std::cerr << "Testing SIMD instructions" << std::endl;

		using vec_type = simd_traits<float>::type;
		size_t vec_size = simd_traits<float>::size;

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

			vec_type cv = av / bv;

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
		unsigned int length = 40;

		a.resize(length, 4.0);
		b.resize(length, 0.0);
		c.resize(length, 0.0);
		result.resize(length, 5.0);

		using vec_type = simd_traits<float>::type;
		size_t vec_size = simd_traits<float>::size;

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
		unsigned int length = 40;

		a.resize(length, 4.0);
		b.resize(length, 0.0);
		c.resize(length, 0.0);
		result.resize(length, 3.0);

		using vec_type = simd_traits<float>::type;
		size_t vec_size = simd_traits<float>::size;

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
		std::uniform_real_distribution<> dis(0,2);

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

			using vec_type = simd_traits<float>::type;
			size_t vec_size = simd_traits<float>::size;

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
		std::uniform_real_distribution<> dis(0,2);

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

			using vec_type = simd_traits<float>::type;
			size_t vec_size = simd_traits<float>::size;

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
		std::uniform_real_distribution<> dis(0,2);

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

			using vec_type = simd_traits<float>::type;
			size_t vec_size = simd_traits<float>::size;

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

	void testDivision(void)
	{
		unsigned int length = 200;
		int runs = 1000;

		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_real_distribution<> dis(0,2);

		for (int run = 0; run < runs; run++)
		{
			vector_type a, b, c;

			for (std::size_t i = 0; i < length; i++)
			{
				a.push_back(dis(gen));
				b.push_back(dis(gen));
			}

			TS_ASSERT_EQUALS(a.size(), b.size());

			auto result = a / b;

			using vec_type = simd_traits<float>::type;
			size_t vec_size = simd_traits<float>::size;

			std::size_t n = a.size();

			c.resize(length, 0.0);

			TS_ASSERT_EQUALS(a.size(), n);
			TS_ASSERT_EQUALS(b.size(), n);

			for(size_t i = 0; i < n; i += vec_size)
			{
				vec_type av = load_a(&a[i]);
				vec_type bv = load_a(&b[i]);

				vec_type cv = av / bv;
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
		std::uniform_real_distribution<> dis(0,2);

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

			using vec_type = simd_traits<float>::type;
			size_t vec_size = simd_traits<float>::size;

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
		std::uniform_real_distribution<> dis(0,2);

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

			using vec_type = simd_traits<float>::type;
			size_t vec_size = simd_traits<float>::size;

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

//	void testLogic1(void)
//	{
//		using vec_type = simd_traits<float>::type;
//		using vec_bool_type = vector4fb;
//		size_t vec_size = simd_traits<float>::size;
//
//		vec_type a(1);
//		vec_type b(1);
//		vec_type c(0);
//	}

//	void testLogic2(void)
//	{
/*
		unsigned int length = 200;
		int runs = 1000;

		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_real_distribution<> dis(0,2);

		for (int run = 0; run < runs; run++)
		{
			vector_type a, b, c, d, result;

			for(std::size_t i = 0; i < length; i++)
			{
				auto number = dis(gen);
				a.push_back(number);
				b.push_back(number);
				c.push_back(number + 0.5);
			}

			using vec_type = simd_traits<float>::type;
			size_t vec_size = simd_traits<float>::size;

			std::size_t n = a.size();

			for(size_t i = 0; i < n; i += vec_size)
			{
				vec_type av = load_a(&a[i]);
				vec_type bv = load_a(&b[i]);
				vec_type cv = load_a(&c[i]);

				TS_ASSERT(av==bv);
				TS_ASSERT(!(av!=bv));

				TS_ASSERT(av!=cv);
				TS_ASSERT(!(av==cv));

				TS_ASSERT(bv!=cv);
				TS_ASSERT(!(bv==cv));
			}
		}
	}

*/
	void testSqrt(void)
	{
		unsigned int length = 200;
		int runs = 1000;

		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_real_distribution<> dis(0,2);

		for (int run = 0; run < runs; run++)
		{
			vector_type a, b;

			for (std::size_t i = 0; i < length; i++)
			{
				a.push_back(dis(gen));
			}

			vector_type result;
			result.resize(length);

			std::transform(a.begin(), a.end(), result.begin(),
						static_cast<float (*)(float)>(std::sqrt));

			TS_ASSERT_EQUALS(a.size(), result.size());

			using vec_type = simd_traits<float>::type;
			size_t vec_size = simd_traits<float>::size;

			std::size_t n = a.size();

			b.resize(length, 0.0);

			TS_ASSERT_EQUALS(a.size(), n);
			TS_ASSERT_EQUALS(b.size(), n);

			for(size_t i = 0; i < n; i += vec_size)
			{
				vec_type av = load_a(&a[i]);

				vec_type bv = sqrt(av);
				store_a(&b[i],bv);
			}

			TS_ASSERT_EQUALS(a.size(), b.size());

			for (std::size_t i = 0; i < length; i++)
			{
				TS_ASSERT_DELTA(result[i], b[i], tol);
			}

		}
	}

	void testSquare(void)
	{
		unsigned int length = 200;
		int runs = 1000;

		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_real_distribution<> dis(0,2);

		for (int run = 0; run < runs; run++)
		{
			vector_type a, b;

			for (std::size_t i = 0; i < length; i++)
			{
				a.push_back(dis(gen));
			}

			vector_type result;
			result.resize(length);

			std::transform(a.begin(), a.end(), result.begin(),
						[] (float d) -> float { return d * d; });

			TS_ASSERT_EQUALS(a.size(), result.size());

			using vec_type = simd_traits<float>::type;
			size_t vec_size = simd_traits<float>::size;

			std::size_t n = a.size();

			b.resize(length, 0.0);

			TS_ASSERT_EQUALS(a.size(), n);
			TS_ASSERT_EQUALS(b.size(), n);

			for(size_t i = 0; i < n; i += vec_size)
			{
				vec_type av = load_a(&a[i]);

				vec_type bv = square(av);
				store_a(&b[i],bv);
			}

			TS_ASSERT_EQUALS(a.size(), b.size());

			for (std::size_t i = 0; i < length; i++)
			{
				TS_ASSERT_DELTA(result[i], b[i], tol);
			}

		}
	}

	void testReciprocal(void)
	{
		// needs higher tolerance -> it's an approx
		double tol = 1E-1;

		unsigned int length = 200;
		int runs = 1000;

		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_real_distribution<> dis(0.1,2);

		for (int run = 0; run < runs; run++)
		{
			vector_type a, b;

			std::vector<double, AlignedAllocator<double, Alignment::AVX>> a_double, result;

			for (std::size_t i = 0; i < length; i++)
			{
				auto value = dis(gen);
				a.push_back(value);
				a_double.push_back(value);
			}

		 	//vector_type tmp, result;
			result.resize(length);

			std::transform(a.begin(), a.end(), result.begin(),
						std::bind1st(std::divides<double>(), 1.0));

			TS_ASSERT_EQUALS(a.size(), result.size());

			using vec_type = simd_traits<float>::type;
			size_t vec_size = simd_traits<float>::size;

			std::size_t n = a.size();

			b.resize(length, 0.0);

			TS_ASSERT_EQUALS(a.size(), n);
			TS_ASSERT_EQUALS(b.size(), n);

			for(size_t i = 0; i < n; i += vec_size)
			{
				vec_type av = load_a(&a[i]);

				vec_type bv = rcp(av);
				store_a(&b[i],bv);
			}

			TS_ASSERT_EQUALS(a.size(), b.size());

			for (std::size_t i = 0; i < length; i++)
			{
				TS_ASSERT_DELTA(result[i], b[i], tol);
			}

		}
	}

	void testRSqrt(void)
	{
		// needs higher tolerance -> it's an approx
		double tol = 1E-1;

		unsigned int length = 200;
		int runs = 1000;

		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_real_distribution<> dis(0.1,2);

		for (int run = 0; run < runs; run++)
		{
			vector_type a, b;

			std::vector<double, AlignedAllocator<double, Alignment::AVX>> a_double, tmp, result;

			for (std::size_t i = 0; i < length; i++)
			{
				auto value = dis(gen);
				a.push_back(value);
				a_double.push_back(value);
			}

		 	//vector_type tmp, result;
			tmp.resize(length);
			result.resize(length);

			std::transform(a.begin(), a.end(), tmp.begin(),
						static_cast<double (*)(double)>(std::sqrt));

			std::transform(tmp.begin(), tmp.end(), result.begin(),
						std::bind1st(std::divides<double>(), 1.0));

			TS_ASSERT_EQUALS(a.size(), tmp.size());
			TS_ASSERT_EQUALS(a.size(), result.size());

			using vec_type = simd_traits<float>::type;
			size_t vec_size = simd_traits<float>::size;

			std::size_t n = a.size();

			b.resize(length, 0.0);

			TS_ASSERT_EQUALS(a.size(), n);
			TS_ASSERT_EQUALS(b.size(), n);

			for(size_t i = 0; i < n; i += vec_size)
			{
				vec_type av = load_a(&a[i]);

				vec_type bv = rsqrt(av);
				store_a(&b[i],bv);
			}

			TS_ASSERT_EQUALS(a.size(), b.size());

			for (std::size_t i = 0; i < length; i++)
			{
				TS_ASSERT_DELTA(result[i], b[i], tol);
			}

		}
	}

	void testAbs(void)
	{
		// needs higher tolerance -> it's an approx
		double tol = 1E-1;

		unsigned int length = 200;
		int runs = 1000;

		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_real_distribution<> dis(-2.0,2.0);

		for (int run = 0; run < runs; run++)
		{
			vector_type a, b;

			std::vector<double, AlignedAllocator<double, Alignment::AVX>> a_double, tmp, result;

			for (std::size_t i = 0; i < length; i++)
			{
				auto value = dis(gen);
				a.push_back(value);
				a_double.push_back(value);
			}

		 	//vector_type tmp, result;
			result.resize(length);

			std::transform(a.begin(), a.end(), result.begin(),
						static_cast<double (*)(double)>(std::abs));

			TS_ASSERT_EQUALS(a.size(), result.size());

			using vec_type = simd_traits<float>::type;
			size_t vec_size = simd_traits<float>::size;

			std::size_t n = a.size();

			b.resize(length, 0.0);

			TS_ASSERT_EQUALS(a.size(), n);
			TS_ASSERT_EQUALS(b.size(), n);

			for(size_t i = 0; i < n; i += vec_size)
			{
				vec_type av = load_a(&a[i]);

				vec_type bv = abs(av);
				store_a(&b[i],bv);
			}

			TS_ASSERT_EQUALS(a.size(), b.size());

			for (std::size_t i = 0; i < length; i++)
			{
				TS_ASSERT_DELTA(result[i], b[i], tol);
			}
		}
	}

	void testHorizonalAdd(void)
	{
		// needs higher tolerance -> it's an approx
		double tol = 1E-1;

		unsigned int length = 200;
		int runs = 1000;

		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_real_distribution<> dis(0,2);

		for (int run = 0; run < runs; run++)
		{
			vector_type a, b;

			for (std::size_t i = 0; i < length; i++)
			{
				a.push_back(dis(gen));
			}

			auto result = std::accumulate(a.begin(), a.end(), 0.0);

			using vec_type = simd_traits<float>::type;
			size_t vec_size = simd_traits<float>::size;

			std::size_t n = a.size();

			float total {0.0};
			for(size_t i = 0; i < n; i += vec_size)
			{
				vec_type av = load_a(&a[i]);

				total += hadd(av);
			}

			TS_ASSERT_DELTA(result, total, tol);
		}
	}

	void testMax(void)
	{
		unsigned int length = 200;
		int runs = 1000;

		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_real_distribution<> dis(0,2);

		for (int run = 0; run < runs; run++)
		{
			vector_type a, b, c;

			for (std::size_t i = 0; i < length; i++)
			{
				a.push_back(dis(gen));
				b.push_back(dis(gen));
			}

			TS_ASSERT_EQUALS(a.size(), b.size());

			vector_type result;

			auto i1 = a.begin(), i2 = b.begin();
			for (; i1 != a.end(); i1++, i2++)
				result.push_back(std::max(*i1, *i2));

			TS_ASSERT_EQUALS(a.size(), result.size());
			TS_ASSERT_EQUALS(b.size(), result.size());

			using vec_type = simd_traits<float>::type;
			size_t vec_size = simd_traits<float>::size;

			std::size_t n = a.size();

			c.resize(length, 0.0);

			TS_ASSERT_EQUALS(a.size(), n);
			TS_ASSERT_EQUALS(b.size(), n);

			for(size_t i = 0; i < n; i += vec_size)
			{
				vec_type av = load_a(&a[i]);
				vec_type bv = load_a(&b[i]);

				vec_type cv = max(av, bv);
				store_a(&c[i],cv);
			}

			TS_ASSERT_EQUALS(c.size(), a.size());
			TS_ASSERT_EQUALS(c.size(), b.size());

			for (std::size_t i = 0; i < length; i++)
			{
				TS_ASSERT_DELTA(result[i], c[i], tol);
			}

		}
	}

	void testMin(void)
	{
		unsigned int length = 200;
		int runs = 1000;

		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_real_distribution<> dis(0,2);

		for (int run = 0; run < runs; run++)
		{
			vector_type a, b, c;

			for (std::size_t i = 0; i < length; i++)
			{
				a.push_back(dis(gen));
				b.push_back(dis(gen));
			}

			TS_ASSERT_EQUALS(a.size(), b.size());

			vector_type result;

			auto i1 = a.begin(), i2 = b.begin();
			for (; i1 != a.end(); i1++, i2++)
				result.push_back(std::min(*i1, *i2));

			TS_ASSERT_EQUALS(a.size(), result.size());
			TS_ASSERT_EQUALS(b.size(), result.size());

			using vec_type = simd_traits<float>::type;
			size_t vec_size = simd_traits<float>::size;

			std::size_t n = a.size();

			c.resize(length, 0.0);

			TS_ASSERT_EQUALS(a.size(), n);
			TS_ASSERT_EQUALS(b.size(), n);

			for(size_t i = 0; i < n; i += vec_size)
			{
				vec_type av = load_a(&a[i]);
				vec_type bv = load_a(&b[i]);

				vec_type cv = min(av, bv);
				store_a(&c[i],cv);
			}

			TS_ASSERT_EQUALS(c.size(), a.size());
			TS_ASSERT_EQUALS(c.size(), b.size());

			for (std::size_t i = 0; i < length; i++)
			{
				TS_ASSERT_DELTA(result[i], c[i], tol);
			}

		}
	}

	void testMulAdd(void)
	{
		unsigned int length = 200;
		int runs = 1000;

		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_real_distribution<> dis(0,2);

		for (int run = 0; run < runs; run++)
		{
			vector_type a, b, c, d;

			for (std::size_t i = 0; i < length; i++)
			{
				a.push_back(dis(gen));
				b.push_back(dis(gen));
				c.push_back(dis(gen));
			}

			TS_ASSERT_EQUALS(a.size(), b.size());
			TS_ASSERT_EQUALS(a.size(), c.size());

			vector_type result;

			for (std::size_t i = 0; i < length; i++)
				result.push_back(a[i] * b[i] + c[i]);

			TS_ASSERT_EQUALS(a.size(), result.size());
			TS_ASSERT_EQUALS(b.size(), result.size());
			TS_ASSERT_EQUALS(c.size(), result.size());

			using vec_type = simd_traits<float>::type;
			size_t vec_size = simd_traits<float>::size;

			std::size_t n = a.size();

			d.resize(length, 0.0);

			TS_ASSERT_EQUALS(a.size(), n);
			TS_ASSERT_EQUALS(b.size(), n);
			TS_ASSERT_EQUALS(c.size(), n);
			TS_ASSERT_EQUALS(d.size(), n);

			for(size_t i = 0; i < n; i += vec_size)
			{
				vec_type av = load_a(&a[i]);
				vec_type bv = load_a(&b[i]);
				vec_type cv = load_a(&c[i]);

				vec_type dv = mul_add(av, bv, cv);
				store_a(&d[i],dv);
			}

			TS_ASSERT_EQUALS(d.size(), a.size());
			TS_ASSERT_EQUALS(d.size(), b.size());
			TS_ASSERT_EQUALS(d.size(), c.size());

			for (std::size_t i = 0; i < length; i++)
			{
				TS_ASSERT_DELTA(result[i], d[i], tol);
			}

		}
	}

	void testMulSub(void)
	{
		unsigned int length = 200;
		int runs = 1000;

		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_real_distribution<> dis(0,2);

		for (int run = 0; run < runs; run++)
		{
			vector_type a, b, c, d;

			for (std::size_t i = 0; i < length; i++)
			{
				a.push_back(dis(gen));
				b.push_back(dis(gen));
				c.push_back(dis(gen));
			}

			TS_ASSERT_EQUALS(a.size(), b.size());
			TS_ASSERT_EQUALS(a.size(), c.size());

			vector_type result;

			for (std::size_t i = 0; i < length; i++)
				result.push_back(a[i] * b[i] - c[i]);

			TS_ASSERT_EQUALS(a.size(), result.size());
			TS_ASSERT_EQUALS(b.size(), result.size());
			TS_ASSERT_EQUALS(c.size(), result.size());

			using vec_type = simd_traits<float>::type;
			size_t vec_size = simd_traits<float>::size;

			std::size_t n = a.size();

			d.resize(length, 0.0);

			TS_ASSERT_EQUALS(a.size(), n);
			TS_ASSERT_EQUALS(b.size(), n);
			TS_ASSERT_EQUALS(c.size(), n);
			TS_ASSERT_EQUALS(d.size(), n);

			for(size_t i = 0; i < n; i += vec_size)
			{
				vec_type av = load_a(&a[i]);
				vec_type bv = load_a(&b[i]);
				vec_type cv = load_a(&c[i]);

				vec_type dv = mul_sub(av, bv, cv);
				store_a(&d[i],dv);
			}

			TS_ASSERT_EQUALS(d.size(), a.size());
			TS_ASSERT_EQUALS(d.size(), b.size());
			TS_ASSERT_EQUALS(d.size(), c.size());

			for (std::size_t i = 0; i < length; i++)
			{
				TS_ASSERT_DELTA(result[i], d[i], tol);
			}

		}
	}

	void testNMulAdd(void)
	{
		unsigned int length = 200;
		int runs = 1000;

		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_real_distribution<> dis(0,2);

		for (int run = 0; run < runs; run++)
		{
			vector_type a, b, c, d;

			for (std::size_t i = 0; i < length; i++)
			{
				a.push_back(dis(gen));
				b.push_back(dis(gen));
				c.push_back(dis(gen));
			}

			TS_ASSERT_EQUALS(a.size(), b.size());
			TS_ASSERT_EQUALS(a.size(), c.size());

			vector_type result;

			for (std::size_t i = 0; i < length; i++)
				result.push_back(-(a[i] * b[i]) + c[i]);

			TS_ASSERT_EQUALS(a.size(), result.size());
			TS_ASSERT_EQUALS(b.size(), result.size());
			TS_ASSERT_EQUALS(c.size(), result.size());

			using vec_type = simd_traits<float>::type;
			size_t vec_size = simd_traits<float>::size;

			std::size_t n = a.size();

			d.resize(length, 0.0);

			TS_ASSERT_EQUALS(a.size(), n);
			TS_ASSERT_EQUALS(b.size(), n);
			TS_ASSERT_EQUALS(c.size(), n);
			TS_ASSERT_EQUALS(d.size(), n);

			for(size_t i = 0; i < n; i += vec_size)
			{
				vec_type av = load_a(&a[i]);
				vec_type bv = load_a(&b[i]);
				vec_type cv = load_a(&c[i]);

				vec_type dv = nmul_add(av, bv, cv);
				store_a(&d[i],dv);
			}

			TS_ASSERT_EQUALS(d.size(), a.size());
			TS_ASSERT_EQUALS(d.size(), b.size());
			TS_ASSERT_EQUALS(d.size(), c.size());

			for (std::size_t i = 0; i < length; i++)
			{
				TS_ASSERT_DELTA(result[i], d[i], tol);
			}

		}
	}

	void testNMulSub(void)
	{
		unsigned int length = 200;
		int runs = 1000;

		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_real_distribution<> dis(0,2);

		for (int run = 0; run < runs; run++)
		{
			vector_type a, b, c, d;

			for (std::size_t i = 0; i < length; i++)
			{
				a.push_back(dis(gen));
				b.push_back(dis(gen));
				c.push_back(dis(gen));
			}

			TS_ASSERT_EQUALS(a.size(), b.size());
			TS_ASSERT_EQUALS(a.size(), c.size());

			vector_type result;

			for (std::size_t i = 0; i < length; i++)
				result.push_back(-(a[i] * b[i]) - c[i]);

			TS_ASSERT_EQUALS(a.size(), result.size());
			TS_ASSERT_EQUALS(b.size(), result.size());
			TS_ASSERT_EQUALS(c.size(), result.size());

			using vec_type = simd_traits<float>::type;
			size_t vec_size = simd_traits<float>::size;

			std::size_t n = a.size();

			d.resize(length, 0.0);

			TS_ASSERT_EQUALS(a.size(), n);
			TS_ASSERT_EQUALS(b.size(), n);
			TS_ASSERT_EQUALS(c.size(), n);
			TS_ASSERT_EQUALS(d.size(), n);

			for(size_t i = 0; i < n; i += vec_size)
			{
				vec_type av = load_a(&a[i]);
				vec_type bv = load_a(&b[i]);
				vec_type cv = load_a(&c[i]);

				vec_type dv = nmul_sub(av, bv, cv);
				store_a(&d[i],dv);
			}

			TS_ASSERT_EQUALS(d.size(), a.size());
			TS_ASSERT_EQUALS(d.size(), b.size());
			TS_ASSERT_EQUALS(d.size(), c.size());

			for (std::size_t i = 0; i < length; i++)
			{
				TS_ASSERT_DELTA(result[i], d[i], tol);
			}

		}
	}



};

