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

class AdhesionPropensitiesUniformTest : public CxxTest::TestSuite
{
private:
    std::shared_ptr<Parameters> p;
	std::unique_ptr<AdhesionPropensities> prop;

	using state_vector = stateVector<unsigned int>;
	using state_vector_type = typename state_vector::storage_type;
	using state_vector_ptr = std::shared_ptr<const state_vector>;

	using float_type = typename simulation_traits::float_type;
	using vec_type = simd_traits<float_type>::type;
	static const std::size_t vec_size = simd_traits<float_type>::size;

	std::shared_ptr<state_vector> state;

    double tol = 1E-5;

	double omega = 0.5;
	double space = 1.0;

	unsigned long runs = 1E6;

public:

    void setUp(void)
    {
		std::vector<double> FinalTimes = {0.1, 0.2, 0.3};
		p = std::make_shared<Parameters>(5, 32, FinalTimes, 1);
        p->setDiffusion(1.0);
        p->setDrift(1.0);
        p->setSensingRadius(1.0);
		p->setRandomWalkType(RANDOMWALK_TYPE::ADHESION);
		p->setOmegaP(1.0);

		// FIXME do this test without a perfect uniform ic
		p->setIcType(IC_TYPE::UNIFORM);
        p->Check();

		state = std::make_shared<state_vector>(p);

        prop = std::make_unique<AdhesionPropensities>();
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

	std::array<double, 2> compute_polarization(long coordinate)
	{
		double diffusion_coeff = p->getDiffusionSim();
		double drift_coeff = p->getDriftSim() * p->getDiscreteX();

		long TotalNumberOfCells = p->getNumberOfCells();

		double total {0.0};
		for (std::size_t i = 1; i < p->getSensingRadiusL() + 1; i++)
		{
			total -= state->getDensityQuick(coordinate - i);
			total += state->getDensityQuick(coordinate + i);
		}

		total *= space;
		total *= omega;

		//TS_ASSERT_DELTA(total, p->getICp() * p->getSensingRadiusL()
	 	//		* space * omega, tol);

		total *= drift_coeff;
		total /= TotalNumberOfCells;

		//std::cout << "total=" << total << std::endl;
		//std::cout << "diff coeff=" << diffusion_coeff << std::endl;

		return {diffusion_coeff + total, diffusion_coeff - total};
	}

	void testVectorSize(void)
	{
		TS_ASSERT_EQUALS(vec_size, 4);
	}

	void test2(void)
	{
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<> dis(0, state->size());

		for (unsigned long run = 0; run < runs; run++)
		{
			long coordinate = dis(gen);
			vec_type rv = prop->adhesivity(coordinate);

			TS_ASSERT_DELTA(rv[0], state->getDensityQuick(coordinate), tol);
			TS_ASSERT_DELTA(rv[1], state->getDensityQuick(coordinate + 1), tol);
			TS_ASSERT_DELTA(rv[2], state->getDensityQuick(coordinate + 2), tol);
			TS_ASSERT_DELTA(rv[3], state->getDensityQuick(coordinate + 3), tol);

			if (rv[0] != (double)state->getDensityQuick(coordinate))
			{
				std::cerr << "rv=(" << rv[0] <<", " << rv[1] << ", "
					<< rv[2] << ", " << rv[3] << ")."
					<< " state=(" << state->getDensityQuick(coordinate)
					<< ", " << state->getDensityQuick(coordinate + 1)
					<< ", " << state->getDensityQuick(coordinate + 2)
					<< ", " << state->getDensityQuick(coordinate + 3)
					<< ")." << std::endl;
				TS_ASSERT(false);
				exit(1);
			}
		}
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
			std::array<double, 2> tmp2 = compute_polarization(coordinate);

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

class AdhesionPropensitiesHeavisideTest : public CxxTest::TestSuite
{
private:
    std::shared_ptr<Parameters> p;
	std::unique_ptr<AdhesionPropensities> prop;

	using state_vector = stateVector<unsigned int>;
	using state_vector_type = typename state_vector::storage_type;
	using state_vector_ptr = std::shared_ptr<const state_vector>;

	using float_type = typename simulation_traits::float_type;
	using vec_type = simd_traits<float_type>::type;
	static const std::size_t vec_size = simd_traits<float_type>::size;

	std::shared_ptr<state_vector> state;

    double tol = 1E-5;

	double omega = 0.5;
	double space = 1.0;

	unsigned long runs = 1E6;

public:

    void setUp(void)
    {
		std::vector<double> FinalTimes = {0.1, 0.2, 0.3};
		p = std::make_shared<Parameters>(5, 32, FinalTimes, 1);
        p->setDiffusion(1.0);
        p->setDrift(10.0);
        p->setSensingRadius(1.0);
		p->setRandomWalkType(RANDOMWALK_TYPE::ADHESION);
		p->setOmegaP(1.0);

		// FIXME do this test without a perfect uniform ic
		p->setIcType(IC_TYPE::HEAVISIDE_RIGHT);
        p->Check();

		state = std::make_shared<state_vector>(p);

        prop = std::make_unique<AdhesionPropensities>();
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

	std::array<double, 2> compute_polarization(long coordinate)
	{
		double diffusion_coeff = p->getDiffusionSim();
		double drift_coeff = p->getDriftSim() * p->getDiscreteX();

		long TotalNumberOfCells = p->getNumberOfCells();

		double total {0.0};
		for (std::size_t i = 1; i < p->getSensingRadiusL() + 1; i++)
		{
			total -= state->getDensityQuick(coordinate - i);
			total += state->getDensityQuick(coordinate + i);
		}

		total *= space;
		total *= omega;

		//TS_ASSERT_DELTA(total, p->getICp() * p->getSensingRadiusL()
	 	//		* space * omega, tol);

		total *= drift_coeff;
		total /= TotalNumberOfCells;

		//std::cout << "total=" << total << std::endl;
		//std::cout << "diff coeff=" << diffusion_coeff << std::endl;

		return {diffusion_coeff + total, diffusion_coeff - total};
	}

	void testVectorSize(void)
	{
		TS_ASSERT_EQUALS(vec_size, 4);
	}

	void testAtCenter(void)
	{
		unsigned long state_length = p->getDomainSizeL();
		unsigned long coordinate = state_length/2 - 1;

		std::cout << "Sensing radius is = " <<
			p->getSensingRadiusL() << std::endl;

		// change density at mid point just to potentially throw of calc
		state->getDensityQuick(coordinate) = 1000;

		std::cout << "Testing at " << coordinate
			<< ". The densities around that point are ("
			<< state->getDensityQuick(coordinate-1) << ", "
			<< state->getDensityQuick(coordinate) << ", "
			<< state->getDensityQuick(coordinate + 1)
			<< ")." << std::endl;

		TS_ASSERT_EQUALS(state->getDensityQuick(coordinate-1), 0);
		TS_ASSERT_EQUALS(state->getDensityQuick(coordinate), 1000);
		TS_ASSERT_EQUALS(state->getDensityQuick(coordinate+1), 1);

		// we don't care about what is at the coordinate
		vec_type rv = prop->adhesivity(coordinate + 1);

		TS_ASSERT_DELTA(rv[0], state->getDensityQuick(coordinate + 1), tol);
		TS_ASSERT_DELTA(rv[1], state->getDensityQuick(coordinate + 2), tol);
		TS_ASSERT_DELTA(rv[2], state->getDensityQuick(coordinate + 3), tol);
		TS_ASSERT_DELTA(rv[3], state->getDensityQuick(coordinate + 4), tol);

		if (rv[0] != (double)state->getDensityQuick(coordinate + 1))
		{
			std::cerr << "rv=(" << rv[0] <<", " << rv[1] << ", "
				<< rv[2] << ", " << rv[3] << ")."
				<< " state=(" << state->getDensityQuick(coordinate)
				<< ", " << state->getDensityQuick(coordinate + 1)
				<< ", " << state->getDensityQuick(coordinate + 2)
				<< ", " << state->getDensityQuick(coordinate + 3)
				<< ")." << std::endl;
			TS_ASSERT(false);
			exit(1);
		}

		std::array<double, 2> tmp = prop->compute(coordinate);
		std::array<double, 2> tmp2 = compute_polarization(coordinate);

		TS_ASSERT_DELTA(tmp[0], tmp2[0], tol);
		TS_ASSERT_DELTA(tmp[1], tmp2[1], tol);

		if ((std::abs(tmp[0] - tmp2[0]) > tol) ||
				(std::abs(tmp[1] - tmp2[1]) > tol))
		{
			std::cerr << "coordinate = " << coordinate
				<< " tmp=(" << tmp[0] << ", " << tmp[1] << ")."
				<< " correct=(" << tmp2[0] << ", " << tmp2[1] << ")."
				<< std::endl;
			TS_ASSERT(false);
			exit(1);
		}
	}

	void test2(void)
	{
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<> dis(0, state->size());

		for (unsigned long run = 0; run < runs; run++)
		{
			long coordinate = dis(gen);
			vec_type rv = prop->adhesivity(coordinate);

			TS_ASSERT_DELTA(rv[0], state->getDensityQuick(coordinate), tol);
			TS_ASSERT_DELTA(rv[1], state->getDensityQuick(coordinate + 1), tol);
			TS_ASSERT_DELTA(rv[2], state->getDensityQuick(coordinate + 2), tol);
			TS_ASSERT_DELTA(rv[3], state->getDensityQuick(coordinate + 3), tol);

			if (rv[0] != (double)state->getDensityQuick(coordinate))
			{
				std::cerr << "rv=(" << rv[0] <<", " << rv[1] << ", "
					<< rv[2] << ", " << rv[3] << ")."
					<< " state=(" << state->getDensityQuick(coordinate)
					<< ", " << state->getDensityQuick(coordinate + 1)
					<< ", " << state->getDensityQuick(coordinate + 2)
					<< ", " << state->getDensityQuick(coordinate + 3)
					<< ")." << std::endl;
				TS_ASSERT(false);
				exit(1);
			}
		}
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
			std::array<double, 2> tmp2 = compute_polarization(coordinate);

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




class AdhesionPropensitiesNonUniformTest : public CxxTest::TestSuite
{
private:
    std::shared_ptr<Parameters> p;
	std::unique_ptr<AdhesionPropensities> prop;

	using state_vector = stateVector<unsigned int>;
	using state_vector_type = typename state_vector::storage_type;
	using state_vector_ptr = std::shared_ptr<const state_vector>;

	using float_type = typename simulation_traits::float_type;
	using vec_type = simd_traits<float_type>::type;
	static const std::size_t vec_size = simd_traits<float_type>::size;

	std::shared_ptr<state_vector> state;

    double tol = 1E-5;

	double omega = 0.5;
	double space = 1.0;

	unsigned long runs = 1E6;

public:

    void setUp(void)
    {
		std::vector<double> FinalTimes = {0.1, 0.2, 0.3};
		p = std::make_shared<Parameters>(5, 32, FinalTimes, 1);
        p->setDiffusion(1.0);
        p->setDrift(1.0);
        p->setSensingRadius(1.0);
		p->setRandomWalkType(RANDOMWALK_TYPE::ADHESION);
		p->setOmegaP(1.0);

		// FIXME do this test without a perfect uniform ic
		p->setIcType(IC_TYPE::DELTA);
        p->Check();

		state = std::make_shared<state_vector>(p);

        prop = std::make_unique<AdhesionPropensities>();
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

	std::array<double, 2> compute_polarization(long coordinate)
	{
		double diffusion_coeff = p->getDiffusionSim();
		double drift_coeff = p->getDriftSim() * p->getDiscreteX();

		long TotalNumberOfCells = p->getNumberOfCells();

		double total {0.0};
		for (std::size_t i = 1; i < p->getSensingRadiusL() + 1; i++)
		{
			total -= state->getDensityQuick(coordinate - i);
			total += state->getDensityQuick(coordinate + i);
		}

		total *= space;
		total *= omega;

		//TS_ASSERT_DELTA(total, p->getICp() * p->getSensingRadiusL()
	 	//		* space * omega, tol);

		total *= drift_coeff;
		total /= TotalNumberOfCells;

		//std::cout << "total=" << total << std::endl;
		//std::cout << "diff coeff=" << diffusion_coeff << std::endl;

		return {diffusion_coeff + total, diffusion_coeff - total};
	}

	void testVectorSize(void)
	{
		TS_ASSERT_EQUALS(vec_size, 4);
	}

	void test2(void)
	{
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<> dis(0, state->size());

		for (unsigned long run = 0; run < runs; run++)
		{
			long coordinate = dis(gen);
			vec_type rv = prop->adhesivity(coordinate);
			vec_type ov = prop->omega(coordinate);
			vec_type fv = prop->space(coordinate);

			TS_ASSERT_DELTA(rv[0], state->getDensityQuick(coordinate), tol);
			TS_ASSERT_DELTA(rv[1], state->getDensityQuick(coordinate + 1), tol);
			TS_ASSERT_DELTA(rv[2], state->getDensityQuick(coordinate + 2), tol);
			TS_ASSERT_DELTA(rv[3], state->getDensityQuick(coordinate + 3), tol);

			TS_ASSERT_DELTA(ov[0], omega, tol);
			TS_ASSERT_DELTA(ov[1], omega, tol);
			TS_ASSERT_DELTA(ov[2], omega, tol);
			TS_ASSERT_DELTA(ov[3], omega, tol);

			TS_ASSERT_DELTA(fv[0], space, tol);
			TS_ASSERT_DELTA(fv[1], space, tol);
			TS_ASSERT_DELTA(fv[2], space, tol);
			TS_ASSERT_DELTA(fv[3], space, tol);

			if (rv[0] != (double)state->getDensityQuick(coordinate))
			{
				std::cerr << "rv=(" << rv[0] <<", " << rv[1] << ", "
					<< rv[2] << ", " << rv[3] << ")."
					<< " state=(" << state->getDensityQuick(coordinate)
					<< ", " << state->getDensityQuick(coordinate + 1)
					<< ", " << state->getDensityQuick(coordinate + 2)
					<< ", " << state->getDensityQuick(coordinate + 3)
					<< ")." << std::endl;
				TS_ASSERT(false);
				exit(1);
			}
		}
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
			std::array<double, 2> tmp2 = compute_polarization(coordinate);

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

class AdhesionPropensitiesRandomTest : public CxxTest::TestSuite
{
private:
    std::shared_ptr<Parameters> p;
	std::unique_ptr<AdhesionPropensities> prop;

	using state_vector = stateVector<unsigned int>;
	using state_vector_type = typename state_vector::storage_type;
	using state_vector_ptr = std::shared_ptr<const state_vector>;

	using float_type = typename simulation_traits::float_type;
	using vec_type = simd_traits<float_type>::type;
	static const std::size_t vec_size = simd_traits<float_type>::size;

	std::shared_ptr<state_vector> state;

    double tol = 1E-5;

	double omega = 0.5;
	double space = 1.0;

	unsigned long runs = 1E6;

public:

    void setUp(void)
    {
		std::vector<double> FinalTimes = {0.1, 0.2, 0.3};
		p = std::make_shared<Parameters>(5, 32, FinalTimes, 1);
        p->setDiffusion(1.0);
        p->setDrift(1.0);
        p->setSensingRadius(1.0);
		p->setRandomWalkType(RANDOMWALK_TYPE::ADHESION);
		p->setOmegaP(1.0);

		// FIXME do this test without a perfect uniform ic
		p->setIcType(IC_TYPE::RANDOM);
        p->Check();

		state = std::make_shared<state_vector>(p);

        prop = std::make_unique<AdhesionPropensities>();
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

	std::array<double, 2> compute_polarization(long coordinate)
	{
		double diffusion_coeff = p->getDiffusionSim();
		double drift_coeff = p->getDriftSim() * p->getDiscreteX();

		long TotalNumberOfCells = p->getNumberOfCells();

		double total {0.0};
		for (std::size_t i = 1; i < p->getSensingRadiusL() + 1; i++)
		{
			total -= state->getDensityQuick(coordinate - i);
			total += state->getDensityQuick(coordinate + i);
		}

		total *= space;
		total *= omega;

		//TS_ASSERT_DELTA(total, p->getICp() * p->getSensingRadiusL()
	 	//		* space * omega, tol);

		total *= drift_coeff;
		total /= TotalNumberOfCells;

		//std::cout << "total=" << total << std::endl;
		//std::cout << "diff coeff=" << diffusion_coeff << std::endl;

		return {diffusion_coeff + total, diffusion_coeff - total};
	}

	void testVectorSize(void)
	{
		TS_ASSERT_EQUALS(vec_size, 4);
	}

	void test2(void)
	{
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<> dis(0, state->size());

		for (unsigned long run = 0; run < runs; run++)
		{
			long coordinate = dis(gen);
			vec_type rv = prop->adhesivity(coordinate);

			TS_ASSERT_DELTA(rv[0], state->getDensityQuick(coordinate), tol);
			TS_ASSERT_DELTA(rv[1], state->getDensityQuick(coordinate + 1), tol);
			TS_ASSERT_DELTA(rv[2], state->getDensityQuick(coordinate + 2), tol);
			TS_ASSERT_DELTA(rv[3], state->getDensityQuick(coordinate + 3), tol);

			if (rv[0] != (double)state->getDensityQuick(coordinate))
			{
				std::cerr << "rv=(" << rv[0] <<", " << rv[1] << ", "
					<< rv[2] << ", " << rv[3] << ")."
					<< " state=(" << state->getDensityQuick(coordinate)
					<< ", " << state->getDensityQuick(coordinate + 1)
					<< ", " << state->getDensityQuick(coordinate + 2)
					<< ", " << state->getDensityQuick(coordinate + 3)
					<< ")." << std::endl;
				TS_ASSERT(false);
				exit(1);
			}
		}
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
			std::array<double, 2> tmp2 = compute_polarization(coordinate);

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
