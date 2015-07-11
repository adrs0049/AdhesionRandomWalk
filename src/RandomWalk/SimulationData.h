#ifndef SIMULATION_DATA_H
#define SIMULATION_DATA_H

#include <vector>

struct SimulationData
{
	typedef std::vector<unsigned int> data_type;

	SimulationData() = delete;
	SimulationData(data_type& _states, unsigned long _steps, double _time)
		: states(_states), steps(_steps), time(_time)
	{}

	std::vector<unsigned int>& states;
	unsigned long steps;
	double time;
};

#endif
