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
#include <AlignmentAllocator.h>
#include "simd.hpp"

class Vector4dTest : public CxxTest::TestSuite
{
private:
    double tol = 1E-5;

	using vector_type = std::vector<double,
		  AlignedAllocator<double, Alignment::AVX>>;

	using vector_bool_type = std::vector<bool,
		  AlignedAllocator<bool, Alignment::AVX>>;

    using int_vector_type = std::vector<int32_t,
          AlignedAllocator<int32_t, Alignment::AVX>>;

public:

    void setUp(void)
    {
		Error::TerminalCatcher::init();
    }

    void tearDown(void)
    {}

    void test_int_to_double(void)
    {
        unsigned long runs = 10000;

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis(-1000,1000);

	for (unsigned long run = 0; run < runs; run++)
	{
		int32_t x[4];

        std::vector<int32_t> values;
		for (std::size_t idx = 0; idx < 4; idx++)
			values.push_back(dis(gen));

		vector4i test(values[0], values[1], values[2], values[3]);
		test.store_u(&x[0]);

		TS_ASSERT_EQUALS(x[0], values[0]);
		TS_ASSERT_EQUALS(x[1], values[1]);
		TS_ASSERT_EQUALS(x[2], values[2]);
		TS_ASSERT_EQUALS(x[3], values[3]);

        TS_ASSERT_EQUALS(test[0], values[0]);
		TS_ASSERT_EQUALS(test[1], values[1]);
		TS_ASSERT_EQUALS(test[2], values[2]);
		TS_ASSERT_EQUALS(test[3], values[3]);

        vector4d double_test = to_double(test);

        TS_ASSERT_DELTA(double_test[0], values[0], tol);
        TS_ASSERT_DELTA(double_test[0], values[0], tol);
        TS_ASSERT_DELTA(double_test[0], values[0], tol);
        TS_ASSERT_DELTA(double_test[0], values[0], tol);
    }

    }

};
