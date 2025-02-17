#ifndef RANDOM_WALK_H
#define RANDOM_WALK_H

#include <iostream>
#include <iomanip>
#include <cmath>
#include <array>
#include "ParkMiller.h"
#include "StateVector.h"
#include "Parameters.h"
#include <memory>
#include "make_unique.h"
#include "Event.h"
#include "Propensities.h"
#include <cstdint>

class Simulator;

#include "Simulator.h"

class RandomWalk
{
	using state_vector = stateVector<int32_t>;
	using state_vector_type = typename state_vector::storage_type;
	using state_vector_ptr = std::shared_ptr<const state_vector>;
	using event_type = Event<state_vector_type>;
    using propensity_type = std::vector<double, AlignedAllocator<double, Alignment::AVX>>;

public:
	RandomWalk();
	RandomWalk(std::shared_ptr<Parameters> _param);
	~RandomWalk();

    RandomWalk(const Simulator& other) = delete;
    RandomWalk(RandomWalk&& other) = delete;
    RandomWalk& operator=(const Simulator& rhs) = delete;
    RandomWalk& operator=(RandomWalk&& rhs) = delete;

    void setSimulator(Simulator* sim_)
    {
        sim = sim_;
    }

	void update(std::shared_ptr<Parameters> _param)
	{
		setup();
	}

	state_vector_type getPath() const
	{
		return state->getStateVector();
	}

	void GeneratePath();
	void Step();

	state_vector_ptr getCells() const
	{
		return state;
	}

	propensity_type& getProp() { return propensities; }
	std::size_t getStride() const { return propensity_stride; }

private:
	void setup();
	void print_info() const;
	void computeAllPropensities();

	std::array<double, 2> getPropensity(long coordinate) const;
	std::vector<double> finalTimes;

	// function to wrap around propensity index
	// when using periodic boundary conditions
	bool propensityApplyCondition(long& idx, const long& max_value);

	// TODO write a test to test if this really works
	void updatePropensity(long coordinate);
	double getPropensity(const int coordinate, const int flag) const;
	double getPropensitySum() const;
	double getPropensitySumQuick() const;
	void computePropensity(long coordinate);

	// Variables
	std::shared_ptr<state_vector> state;
	std::shared_ptr<Parameters> param;
	std::unique_ptr<PropensitiesGenerator> pgen;

	// Propensity vector
   	propensity_type propensities;
	long propensity_stride;
	unsigned long sensing_offset;

	unsigned long steps;
	double time;

	unsigned long NumberOfReactions;
	UniformRandomNumberGenerator rand;
    Simulator* sim;
};

#endif
