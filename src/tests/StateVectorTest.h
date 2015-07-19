// test
#include <cxxtest/TestSuite.h>

#include <iostream>
#include "Parameters.h"
#include <memory>
#include <iterator>
#include <vector>
#include <algorithm>
#include "debug.h"
#include <array>

#include <make_unique.h>

#include <iterator>
#include <random>

#define private public
#include "StateVector.h"

class StateVectorTest : public CxxTest::TestSuite
{
private:
    std::shared_ptr<Parameters> p;

	using state_vector = stateVector<unsigned int>;
	using state_vector_type = typename state_vector::storage_type;
	using state_vector_ptr = std::shared_ptr<const state_vector>;

	std::shared_ptr<state_vector> state;

    double tol = 1E-5;
	unsigned long runs = 1E6;

public:

    void setUp(void)
    {
		std::vector<double> FinalTimes = {0.1, 0.2, 0.3};
		p = std::make_shared<Parameters>(5, 32, FinalTimes, 1);
        p->setDiffusion(1.0);
        p->setDrift(1.0);
        p->setSensingRadius(1.0);
		p->setRandomWalkType(RANDOMWALK_TYPE::DIFFUSION);
    }

    void tearDown(void)
    {
		state.reset();
		p.reset();
    }

	void testConstructUniform(void)
	{
		p->setIcType(IC_TYPE::UNIFORM);
		p->Check();

		state = std::make_shared<state_vector>(p);

		unsigned long ic_p = p->getICp();

		for (std::size_t idx = 0; idx < state->size(); idx++)
		{
			TS_ASSERT_EQUALS(state->getDensityQuick(idx), ic_p);
		}
	}

	void testConstructDelta(void)
	{
		p->setIcType(IC_TYPE::DELTA);
		p->Check();

		state = std::make_shared<state_vector>(p);

		unsigned long ic_p = p->getICp();
		unsigned long mid = p->getDomainSizeL()/2 - 1;

		for (std::size_t idx = 0; idx < state->size(); idx++)
		{
			if (idx == mid || idx == (mid - 1) || idx == (mid + 1))
			{
				TS_ASSERT_EQUALS(state->getDensityQuick(idx), ic_p);
			}
			else
			{
				TS_ASSERT_EQUALS(state->getDensityQuick(idx), 0);
			}
		}
	}

	void testConstructHeavisideLeft(void)
	{
		p->setIcType(IC_TYPE::HEAVISIDE_LEFT);
		p->Check();

		state = std::make_shared<state_vector>(p);

		unsigned long ic_p = p->getICp();
		unsigned long mid = p->getDomainSizeL()/2 - 1;

		for (std::size_t idx = 0; idx < mid + 1; idx++)
		{
			TS_ASSERT_EQUALS(state->getDensityQuick(idx), ic_p);
		}
		for (std::size_t idx = mid + 1; idx < state->size(); idx++)
		{
			TS_ASSERT_EQUALS(state->getDensityQuick(idx), 0);
		}
	}

	void testConstructHeavisideRight(void)
	{
		p->setIcType(IC_TYPE::HEAVISIDE_RIGHT);
		p->Check();

		state = std::make_shared<state_vector>(p);

		unsigned long ic_p = p->getICp();
		unsigned long mid = p->getDomainSizeL()/2 - 1;

		for (std::size_t idx = 0; idx < mid; idx++)
		{
			TS_ASSERT_EQUALS(state->getDensityQuick(idx), 0);
		}
		for (std::size_t idx = mid; idx < state->size(); idx++)
		{
			TS_ASSERT_EQUALS(state->getDensityQuick(idx), ic_p);
		}
	}

	void testShiftLeft(void)
	{
		p->setIcType(IC_TYPE::UNIFORM);
		p->Check();

		state = std::make_shared<state_vector>(p);

		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<> dis(0, state->size()-1);

		for (std::size_t run = 0; run < runs; run++)
		{
			long coordinate = dis(gen);
			TS_ASSERT_LESS_THAN(coordinate, state->size());
			TS_ASSERT_LESS_THAN_EQUALS(0, coordinate);

			state->LeftShift(coordinate);

			if (coordinate == 0)
			{
				TS_ASSERT_EQUALS(state->getDensity(coordinate), 0);
				TS_ASSERT_EQUALS(state->getDensity(p->getDomainSizeL()-1), 2);

				for (std::size_t idx = 1; idx < p->getDomainSizeL()-1; idx++)
				{
					TS_ASSERT_EQUALS(state->getDensity(idx), 1);
				}

				// restore
				state->RightShift(p->getDomainSizeL()-1);

				for (std::size_t idx = 0; idx < state->size(); idx++)
					TS_ASSERT_EQUALS(state->getDensity(idx), 1);
			}
			else
			{
				// verification of shift
				for (long idx = 0; idx < coordinate - 1; idx++)
				{
					TS_ASSERT_EQUALS(state->getDensity(idx), 1);
				}

				for (std::size_t idx = coordinate + 1; idx < state->size(); idx++)
				{
					TS_ASSERT_EQUALS(state->getDensity(idx), 1);
				}

				TS_ASSERT_EQUALS(state->getDensity(coordinate - 1), 2);
				TS_ASSERT_EQUALS(state->getDensity(coordinate), 0);

				// now shift back
				state->RightShift(coordinate - 1);

				// verify all is 1 again
				for (std::size_t idx = 0; idx < state->size(); idx++)
					TS_ASSERT_EQUALS(state->getDensity(idx), 1);
			}
		}
	}

	void testShiftRight(void)
	{
		p->setIcType(IC_TYPE::UNIFORM);
		p->Check();

		state = std::make_shared<state_vector>(p);

		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<> dis(0, state->size()-1);

		for (std::size_t run = 0; run < runs; run++)
		{
			long coordinate = dis(gen);
			TS_ASSERT_LESS_THAN(coordinate, state->size());
			TS_ASSERT_LESS_THAN_EQUALS(0, coordinate);

			state->RightShift(coordinate);

			if (coordinate == (long)(p->getDomainSizeL() - 1))
			{
				TS_ASSERT_EQUALS(state->getDensity(coordinate), 0);
				TS_ASSERT_EQUALS(state->getDensity(0), 2);

				for (long idx = 1; idx < coordinate; idx++)
				{
					TS_ASSERT_EQUALS(state->getDensity(idx), 1);
				}

				// restore
				state->LeftShift(0);

				for (std::size_t idx = 0; idx < state->size(); idx++)
					TS_ASSERT_EQUALS(state->getDensity(idx), 1);
			}
			else
			{
				// verification of shift
				for (long idx = 0; idx < coordinate; idx++)
				{
					TS_ASSERT_EQUALS(state->getDensity(idx), 1);
					if (state->getDensity(idx) != 1)
						exit(1);
				}

				for (std::size_t idx = coordinate + 2; idx < state->size(); idx++)
				{
					TS_ASSERT_EQUALS(state->getDensity(idx), 1);
				}

				TS_ASSERT_EQUALS(state->getDensity(coordinate + 1), 2);
				TS_ASSERT_EQUALS(state->getDensity(coordinate), 0);

				// now shift back
				state->LeftShift(coordinate + 1);

				// verify all is 1 again
				for (std::size_t idx = 0; idx < state->size(); idx++)
					TS_ASSERT_EQUALS(state->getDensity(idx), 1);
			}
		}
	}
};
