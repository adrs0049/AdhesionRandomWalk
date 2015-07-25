#ifndef SIMULATION_DATA_H
#define SIMULATION_DATA_H

#include "vector.h"

struct SimulationData
{
	typedef AligndVector<int32_t> data_type;
	//typedef std::vector<unsigned int> data_type;

	SimulationData() = delete;
	SimulationData(data_type _states, unsigned long _steps, double _time)
		: states(_states), steps(_steps), time(_time)
	{}

	data_type states;
	unsigned long steps;
	double time;
};

#endif
