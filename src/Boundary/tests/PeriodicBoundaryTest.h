// test
#include <cxxtest/TestSuite.h>

#include <random>

#include <iostream>

#include <Boundary.h>
#include <PeriodicBoundary.h>
#include <BoundaryFactory.h>

class PeriodicBoundaryTest : public CxxTest::TestSuite
{
private:

	boundary::BOUNDARY_TYPE type = boundary::BOUNDARY_TYPE::PERIODIC;
	boundary::Boundary * b;

    double tol = 1E-5;

public:

    void setUp(void)
    {
		b = boundary::BoundaryFactory::createBoundary(type);
    }

    void tearDown(void)
    {
		free(b);
    }

	void testBasic(void)
	{
		long domainSize = 100;
		unsigned long runs = 100000;

		std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(-10 * domainSize, 10 * domainSize);

		int coordinate = 0;
		int before = 0;

		for (unsigned long run = 0; run < runs; run++)
		{
			coordinate = before = dis(gen);

			auto ret = b->applyCondition(coordinate, domainSize);

			if (ret) // things have changed
			{
				if (coordinate == domainSize)
				{
					std::cout << "Things have changed! ";
					std::cout << "(before, after)=" << before << ", "
						<< coordinate << "." << std::endl;
				}

				TS_ASSERT_LESS_THAN(coordinate, domainSize);
				TS_ASSERT_LESS_THAN_EQUALS(0, coordinate);
			}
			else
			{
				if (coordinate == domainSize)
				{
					std::cout << "Things haven't changed! ";
					std::cout << "(before, after)=" << before << ", "
						<< coordinate << "." << std::endl;
				}

				TS_ASSERT_LESS_THAN(coordinate, domainSize);
				TS_ASSERT_LESS_THAN_EQUALS(0, coordinate);
			}
		}
	}

	void testPeriodicNegativeCoordinates(void)
	{
		long domainSize = 100;

		int coordinate, before = 0;
		for (long idx = -10000; idx <= 0; idx++)
		{
			coordinate = before = idx;

			auto ret = b->applyCondition(coordinate, domainSize);

			if (ret) // things have changed
			{
				if (coordinate == domainSize)
				{
					std::cout << "Things have changed! ";
					std::cout << "(before, after)=" << before << ", "
						<< coordinate << "." << std::endl;
				}

				TS_ASSERT_LESS_THAN(coordinate, domainSize);
				TS_ASSERT_LESS_THAN_EQUALS(0, coordinate);
			}
			else
			{
				if (coordinate == domainSize)
				{
					std::cout << "Things haven't changed! ";
					std::cout << "(before, after)=" << before << ", "
						<< coordinate << "." << std::endl;
				}

				TS_ASSERT_LESS_THAN(coordinate, domainSize);
				TS_ASSERT_LESS_THAN_EQUALS(0, coordinate);
			}

			if ( (std::abs(before) % domainSize) == 0)
			{
				TS_ASSERT_EQUALS(coordinate, 0);
			}
			else
			{
				TS_ASSERT_EQUALS(coordinate,
						domainSize - (std::abs(before) % domainSize ));
			}

		}
	}

	void testPeriodicPositiveCoordinates(void)
	{
		long domainSize = 100;

		int coordinate, before = 0;
		for (long idx = 0; idx <= 10000; idx++)
		{
			coordinate = before = idx;

			auto ret = b->applyCondition(coordinate, domainSize);

			if (ret) // things have changed
			{
				if (coordinate == domainSize)
				{
					std::cout << "Things have changed! ";
					std::cout << "(before, after)=" << before << ", "
						<< coordinate << "." << std::endl;
				}

				TS_ASSERT_LESS_THAN(coordinate, domainSize);
				TS_ASSERT_LESS_THAN_EQUALS(0, coordinate);
			}
			else
			{
				if (coordinate == domainSize)
				{
					std::cout << "Things haven't changed! ";
					std::cout << "(before, after)=" << before << ", "
						<< coordinate << "." << std::endl;
				}

				TS_ASSERT_LESS_THAN(coordinate, domainSize);
				TS_ASSERT_LESS_THAN_EQUALS(0, coordinate);
			}

			if ( (std::abs(before) % domainSize) == 0)
			{
				TS_ASSERT_EQUALS(coordinate, 0);
			}
			else
			{
				TS_ASSERT_EQUALS(coordinate,
							(std::abs(before) % domainSize ));
			}

		}
	}

};
