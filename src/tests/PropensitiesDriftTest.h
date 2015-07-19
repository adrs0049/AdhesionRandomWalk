// test
#include <cxxtest/TestSuite.h>

#include <iostream>
#include "Parameters.h"
#include "StateVector.h"
#include <memory>
#include <iterator>
#include <vector>
#include <algorithm>
#include "debug.h"
#include <array>

#include <make_unique.h>

#include <iterator>
#include <random>

#include "Event.h"
#include "EventListener.h"

#include "Terminate.h"
#include <simd.hpp>

#define private public
#include "Propensities.h"

class DriftPropensitiesUniformTest : public CxxTest::TestSuite
{
private:
    std::shared_ptr<Parameters> p;
	std::unique_ptr<DriftPropensities> prop;

	using state_vector = stateVector<unsigned int>;
	using state_vector_type = typename state_vector::storage_type;
	using state_vector_ptr = std::shared_ptr<const state_vector>;

	using float_type = typename simulation_traits::float_type;
	using vec_type = simd_traits<float_type>::type;
	static const std::size_t vec_size = simd_traits<float_type>::size;

	std::shared_ptr<state_vector> state;

    double tol = 1E-5;

public:

    void setUp(void)
    {
		std::vector<double> FinalTimes = {0.1, 0.2, 0.3};
		p = std::make_shared<Parameters>(5, 32, FinalTimes, 1);
        p->setDiffusion(1.0);
        p->setDrift(1.0);
        p->setSensingRadius(1.0);
		p->setRandomWalkType(RANDOMWALK_TYPE::DIFFUSION_AND_DRIFT);
		p->setIcType(IC_TYPE::UNIFORM);
        p->Check();

		state = std::make_shared<state_vector>(p);

        prop = std::make_unique<DriftPropensities>();
		prop->set(p);
		prop->set(state.get());
		prop->verify();
    }

    void tearDown(void)
    {
		prop.reset();
		state.reset();
		p.reset();
    }

	std::array<double, 2> compute_propensities(long coordinate)
	{
		double diffusion_coeff = p->getDiffusionSim();
		double drift_coeff = p->getDriftSim() * p->getDiscreteX();

		return {diffusion_coeff + drift_coeff, diffusion_coeff - drift_coeff};
	}

	void testVectorSize(void)
	{
		TS_ASSERT_EQUALS(vec_size, 4);
	}

    void testBasic(void)
    {
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<> dis(0, state->size());

		unsigned long runs = 100000;

		for (unsigned long run = 0; run < runs; run++)
		{
			long coordinate = dis(gen);

			std::array<double, 2> tmp = prop->compute(coordinate);
			std::array<double, 2> tmp2 = compute_propensities(coordinate);

			TS_ASSERT_DELTA(tmp[0], tmp2[0], tol);
			TS_ASSERT_DELTA(tmp[1], tmp2[1], tol);

			if ((std::abs(tmp[0] - tmp2[0]) > tol) ||
				(std::abs(tmp[1] - tmp2[1]) > tol))
			{
				std::cerr << "run=" << run << " " ;
				std::cerr << "coordinate = " << coordinate
					<< " tmp=(" << tmp[0] << ", " << tmp[1] << ")."
					<< " tmp2=(" << tmp2[0] << ", " << tmp2[1] << ")."
					<< std::endl;
				TS_ASSERT(false);
				exit(1);
			}
		}
    }
};

class DriftPropensitiesNonUniformTest : public CxxTest::TestSuite
{
private:
    std::shared_ptr<Parameters> p;
	std::unique_ptr<DriftPropensities> prop;

	using state_vector = stateVector<unsigned int>;
	using state_vector_type = typename state_vector::storage_type;
	using state_vector_ptr = std::shared_ptr<const state_vector>;

	using float_type = typename simulation_traits::float_type;
	using vec_type = simd_traits<float_type>::type;
	static const std::size_t vec_size = simd_traits<float_type>::size;

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
		p->setRandomWalkType(RANDOMWALK_TYPE::DIFFUSION_AND_DRIFT);
		p->setIcType(IC_TYPE::DELTA);
        p->Check();

		state = std::make_shared<state_vector>(p);

        prop = std::make_unique<DriftPropensities>();
		prop->set(p);
		prop->set(state.get());
		prop->verify();
    }

    void tearDown(void)
    {
		prop.reset();
		state.reset();
		p.reset();
    }

	std::array<double, 2> compute_propensities(long coordinate)
	{
		double diffusion_coeff = p->getDiffusionSim();
		double drift_coeff = p->getDriftSim() * p->getDiscreteX();

		return {diffusion_coeff + drift_coeff, diffusion_coeff - drift_coeff};
	}

	void testVectorSize(void)
	{
		TS_ASSERT_EQUALS(vec_size, 4);
	}

    void testBasic(void)
    {
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<> dis(0, state->size());

		for (unsigned long run = 0; run < runs; run++)
		{
			long coordinate = dis(gen);

			std::array<double, 2> tmp = prop->compute(coordinate);
			std::array<double, 2> tmp2 = compute_propensities(coordinate);

			TS_ASSERT_DELTA(tmp[0], tmp2[0], tol);
			TS_ASSERT_DELTA(tmp[1], tmp2[1], tol);

			if ((std::abs(tmp[0] - tmp2[0]) > tol) ||
				(std::abs(tmp[1] - tmp2[1]) > tol))
			{
				std::cerr << "run=" << run << " " ;
				std::cerr << "coordinate = " << coordinate
					<< " tmp=(" << tmp[0] << ", " << tmp[1] << ")."
					<< " tmp2=(" << tmp2[0] << ", " << tmp2[1] << ")."
					<< std::endl;
				TS_ASSERT(false);
				exit(1);
			}
		}
    }
};

class DriftPropensitiesRandomTest : public CxxTest::TestSuite
{
private:
    std::shared_ptr<Parameters> p;
	std::unique_ptr<DriftPropensities> prop;

	using state_vector = stateVector<unsigned int>;
	using state_vector_type = typename state_vector::storage_type;
	using state_vector_ptr = std::shared_ptr<const state_vector>;

	using float_type = typename simulation_traits::float_type;
	using vec_type = simd_traits<float_type>::type;
	static const std::size_t vec_size = simd_traits<float_type>::size;

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
		p->setRandomWalkType(RANDOMWALK_TYPE::DIFFUSION_AND_DRIFT);
		p->setIcType(IC_TYPE::RANDOM);
        p->Check();

		state = std::make_shared<state_vector>(p);

        prop = std::make_unique<DriftPropensities>();
		prop->set(p);
		prop->set(state.get());
		prop->verify();
    }

    void tearDown(void)
    {
		prop.reset();
		state.reset();
		p.reset();
    }

	std::array<double, 2> compute_propensities(long coordinate)
	{
		double diffusion_coeff = p->getDiffusionSim();
		double drift_coeff = p->getDriftSim() * p->getDiscreteX();

		return {diffusion_coeff + drift_coeff, diffusion_coeff - drift_coeff};
	}

	void testVectorSize(void)
	{
		TS_ASSERT_EQUALS(vec_size, 4);
	}

    void testBasic(void)
    {
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<> dis(0, state->size());

		for (unsigned long run = 0; run < runs; run++)
		{
			long coordinate = dis(gen);

			std::array<double, 2> tmp = prop->compute(coordinate);
			std::array<double, 2> tmp2 = compute_propensities(coordinate);

			TS_ASSERT_DELTA(tmp[0], tmp2[0], tol);
			TS_ASSERT_DELTA(tmp[1], tmp2[1], tol);

			if ((std::abs(tmp[0] - tmp2[0]) > tol) ||
				(std::abs(tmp[1] - tmp2[1]) > tol))
			{
				std::cerr << "run=" << run << " " ;
				std::cerr << "coordinate = " << coordinate
					<< " tmp=(" << tmp[0] << ", " << tmp[1] << ")."
					<< " tmp2=(" << tmp2[0] << ", " << tmp2[1] << ")."
					<< std::endl;
				TS_ASSERT(false);
				exit(1);
			}
		}
    }
};


