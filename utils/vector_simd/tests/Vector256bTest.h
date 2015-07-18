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
#include <simd.hpp>
#include <simd_traits_avx.h>
#include "Vec256.h"
#include <AlignmentAllocator.h>

#include "vector_ops.h"
#include <cstdlib>

class Vector256bTest : public CxxTest::TestSuite
{
private:
    double tol = 1E-5;

	using vector_type = std::vector<bool,
		  AlignedAllocator<bool, Alignment::AVX>>;

public:

    void setUp(void)
    {
        std::cerr << "setup" << std::endl;
		Error::TerminalCatcher::init();
    }

    void tearDown(void)
    {
        std::cerr << "TearDown" << std::endl;
    }

	void testBasic(void)
	{
		vector256b test;
	}

	void testBasic2(void)
	{
		vector8i test;
	}
};

