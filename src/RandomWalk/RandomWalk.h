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

class RandomWalk
{
	using state_vector = stateVector<unsigned int>;
	using state_vector_type = typename state_vector::storage_type;
	using state_vector_ptr = std::shared_ptr<state_vector>;
	using event_type = Event<state_vector_type>;

public:
	RandomWalk()
		: state(nullptr), param(nullptr)
	{}

	RandomWalk( std::shared_ptr<Parameters> _param )
		: state(std::make_shared<state_vector>(_param, "Uniform")),
		param(_param),
		rand ( 0.0, 1.0 )
	{}

	~RandomWalk() {}

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

	state_vector_ptr getCells()
	{
		return state;
	}

	std::vector<double>& getProp() { return propensities; }
	std::size_t getStride() { return propensity_stride; }

public:
	void setup();
	void print_info();
	void computeAllPropensities();

	void notify()
	{
		event.notifyListeners(getPath());
	}

	event_type event;

	std::array<double, 2> getPropensity(long coordinate);
	std::vector<double> finalTimes;

	// function to wrap around propensity index
	// when using periodic boundary conditions
	bool propensityApplyCondition(long& idx, const long& max_value);

	// TODO write a test to test if this really works
	void updatePropensity(long coordinate);
	double getPropensity(int coordinate, int flag);
	double getPropensitySum();
	void computePropensity( long coordinate );

	// TODO think of something better for these functions
	double omega(long coordinate);
	double adhesivity(long coordinate);
	double space(long coordinate);
	double PolarizationVector(long coordinate);

	unsigned long TotalNumberOfCells;

	// Variables
	std::shared_ptr<state_vector> state;
	std::shared_ptr<Parameters> param;

	// Propensity vector
	std::vector<double> propensities;
	long propensity_stride;
	unsigned long sensing_offset;

	unsigned long steps;
	double time;

	unsigned long right;
	unsigned long left;
	unsigned long NumberOfReactions;
	unsigned long NumberOfAttemptFlips;
	unsigned long NumberOfUnsuccessfulFlips;
	unsigned int Multiplier = 1;
	UniformRandomNumberGenerator rand;
};

#endif
