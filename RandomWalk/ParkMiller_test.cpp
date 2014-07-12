#define BOOST_TEST_MODULE ParkMiller test 
#include <boost/test/unit_test.hpp>

// redefine private to public allows unit testing of private stuff
#define private public
#include "ParkMiller.h"

BOOST_AUTO_TEST_CASE( constructor_test )
{
	ParkMiller gen; 				/* 1 */
	BOOST_CHECK_EQUAL(gen.Seed, 1 );
	BOOST_CHECK_EQUAL(gen.Max(), ParkMiller::random_engine::max() );
	BOOST_CHECK_EQUAL(gen.Min(), ParkMiller::random_engine::min() );
	
	ParkMiller gen2(2); 				/* 2 */
	BOOST_CHECK_EQUAL(gen2.Seed, 2 );
	BOOST_CHECK_EQUAL(gen2.Max(), ParkMiller::random_engine::max() );
	BOOST_CHECK_EQUAL(gen2.Min(), ParkMiller::random_engine::min() );
	
	ParkMiller gen3(0);				/* 3 */
	BOOST_CHECK_EQUAL(gen3.Seed, 1 );
	BOOST_CHECK_EQUAL(gen3.Max(), ParkMiller::random_engine::max() );
	BOOST_CHECK_EQUAL(gen3.Min(), ParkMiller::random_engine::min() );
}

BOOST_AUTO_TEST_CASE( random_number_generator_test )
{
	/* Initialise not really a test */
	
	ParkMiller gen; 				/* 1 */
	BOOST_CHECK_EQUAL(gen.Seed, 1 );
	BOOST_CHECK_EQUAL(gen.Max(), ParkMiller::random_engine::max() );
	BOOST_CHECK_EQUAL(gen.Min(), ParkMiller::random_engine::min() );
	
	/* Random Test */
	
	BOOST_CHECK_EQUAL( gen.GetOneRandomInteger(), 48271 );
	
	gen.Skip();
	BOOST_CHECK_EQUAL( gen.GetOneRandomInteger(), 1291394886 );
	
	gen.Skip(10);
	BOOST_CHECK_EQUAL( gen.GetOneRandomInteger(), 1738531149 );
	
	/* Set Seed Test */
	gen.SetSeed(10); 				/* 1  */
	BOOST_CHECK_EQUAL(gen.Seed, 10 );
	BOOST_CHECK_EQUAL( gen.GetOneRandomInteger(), 482710 );

	gen.SetSeed(0);					/* 2 */
	BOOST_CHECK_EQUAL(gen.Seed, 1 );
	BOOST_CHECK_EQUAL( gen.GetOneRandomInteger(), 48271 );
}