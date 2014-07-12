#define BOOST_TEST_MODULE Statistics test 
#include <boost/test/unit_test.hpp>

// redefine private to public allows unit testing of private stuff
#define private public
#include "Statistics.h"

#define ITERATIONS 100

BOOST_AUTO_TEST_CASE( constructor_test )
{
	StatisticsMean mean_test;
	BOOST_CHECK_EQUAL(mean_test.RunningSum, 0.0);
	BOOST_CHECK_EQUAL(mean_test.PathsDone, 0);
	
	auto new_test = mean_test.clone();
	BOOST_CHECK_EQUAL(new_test->RunningSum, 0.0);
	BOOST_CHECK_EQUAL(new_test->PathsDone, 0);
}

BOOST_AUTO_TEST_CASE( random_number_generator_test )
{
	const double tol = 1E-6;
	
	StatisticsMean test;
	BOOST_CHECK_EQUAL(test.RunningSum, 0.0);
	BOOST_CHECK_EQUAL(test.PathsDone, 0);
	
	for (unsigned long i = 0; i < ITERATIONS; i++)
		test.DumpOneResult(i);
	
	BOOST_CHECK_EQUAL(test.RunningSum, 4950.0);
	BOOST_CHECK_EQUAL(test.PathsDone, 100);
	
	std::vector<std::vector<double>> results = test.GetResultsSoFar();
	BOOST_CHECK_CLOSE(49.50, results[0][0], tol);
}
