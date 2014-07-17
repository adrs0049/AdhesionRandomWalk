#define BOOST_TEST_MODULE RandomWalk test 
#include <boost/test/unit_test.hpp>

// redefine private to public allows unit testing of private stuff
#define private public

#include "array.h"

#define NDEBUG
#include <debug.h>

#include "ParkMiller.h"
#include "RandomWalkBase.h"
#include "RandomWalkStatistics.h"
#include "RandomWalk.h"

#include <ctime>
#include <chronos.h>
using chronos::Chronos;
Chronos keeper;

#include <iostream>
#include <iomanip>

BOOST_AUTO_TEST_CASE( constructor_test )
{
	unsigned long numbersToGenerate = 5;
	RandomWalkStatistics stats;
	
	iArray values(numbersToGenerate);
	RandomParkMiller random_number_engine(numbersToGenerate, time(NULL));
	
	RandomWalk rw ({1,2,3,4}, numbersToGenerate);
	
	DEBUG("create simplerandomwalk\n");
	SimpleRandomWalk test2(random_number_engine, rw);
	
// 	test2.GetOnePath(values);
	
// 	PRINT_VAL(values);
	
	DEBUG("Do simulation now\n");
	
	test2.DoSimulation(stats, 5);
	
	DEBUG("STATS\n");
	stats.GetResultsSoFar();
// 	auto results = stats.GetResultsSoFar();
// 	std::cout << "Mean="<< results[0][0] << " Variance="<<results[0][1] << std::endl;
}

/*
BOOST_AUTO_TEST_CASE( random_number_generator_test )
{
	
}
*/