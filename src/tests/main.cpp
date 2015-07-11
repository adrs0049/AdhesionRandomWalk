#include <iostream>

#include "Simulator.h"
#include <memory>

int main()
{

	Parameters p (10.0, 0.01, 0.005, 1.0);
	Simulator sim (std::make_shared<Parameters>(p));

	//sim.init();

	sim.run();
	sim.run();
	sim.run();

	return 0;
}

