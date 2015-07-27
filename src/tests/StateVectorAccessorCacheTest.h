// test
#include <cxxtest/TestSuite.h>

#include <iostream>
#include "Parameters.h"
#include "Terminate.h"
#include <memory>
#include <iterator>
#include <vector>
#include <algorithm>
#include "debug.h"
#include <array>

#include <make_unique.h>
#include <iomanip>

#include <iterator>
#include <random>

#include "StateVector.h"

#define private public
#include "StateVectorAccessorCache.h"

class StateVectorAccessorTest : public CxxTest::TestSuite
{
private:
    std::shared_ptr<Parameters> p;

	using state_vector = stateVector<unsigned int>;
	using state_vector_type = typename state_vector::storage_type;
	using state_vector_ptr = std::shared_ptr<const state_vector>;

	std::shared_ptr<state_vector> state;
	std::shared_ptr<StateVectorAccessorCache> cache;

    double tol = 1E-5;
	unsigned long runs = 1E6;

	int begin() const { return - p->getSensingRadiusL(); }
	int end() const { return state->size() + p->getSensingRadiusL() + 1; }
	int sbegin() const { return -p->getSensingRadiusL(); }
	int send() const { return p->getSensingRadiusL() + 1; }

public:

    void setUp(void)
    {
		Error::TerminalCatcher::init();
		std::vector<double> FinalTimes = {0.1, 0.2, 0.3};
		p = std::make_shared<Parameters>(5, 32, FinalTimes);
        p->setDiffusion(1.0);
        p->setDrift(1.0);
        p->setSensingRadius(1.0);
		p->setRandomWalkType(RANDOMWALK_TYPE::DIFFUSION);
        p->setIcP(1);
		p->setIcType(IC_TYPE::RANDOM);
		p->update();
		p->Check();

		state = std::make_shared<state_vector>(p);
		cache = std::make_shared<StateVectorAccessorCache>(p);
		cache->setup(p);
    }

    void tearDown(void)
    {
		state.reset();
		p.reset();
    }

	void testConstructUniform(void)
	{
		// verify data
		for (int coordinate = - p->getSensingRadiusL();
				 coordinate < state->size() + p->getSensingRadiusL() + 1; coordinate++)
		{
			int c = cache->operator()(coordinate, 0);
			TS_ASSERT_EQUALS(state->getDensityQuick(coordinate), state->getDensityDirect(c));
		}
	}

	void testVerifyCacheData(void)
	{
		for (int coordinate = begin(); coordinate < end(); coordinate++)
		{
			for (int offset = sbegin(); offset < send(); offset++)
			{
				//std::cout << "(coord, off)=" <<coordinate << ", " <<offset << ")";
				int offset_tmp  = coordinate + offset;
				//std::cout << " off_tmp=" << offset_tmp;
				int c = cache->operator()(coordinate, offset);
				state->applyCondition(offset_tmp);
				//std::cout << " off_temp_after=" << offset_tmp << " c=" << c << std::endl;
				TS_ASSERT_EQUALS(c, offset_tmp);

				// also verify that data match
				TS_ASSERT_EQUALS(state->getDensityQuick(coordinate + offset),
						state->getDensityDirect(c));
			}
		}
	}

};
