#include <iostream>

#include "Simulator.h"
#include <memory>

const std::string BoundaryFactory::no_flux("NoFlux");
const std::string BoundaryFactory::periodic("Periodic");

const std::string CellDensity::delta("Delta");
const std::string CellDensity::uniform("Uniform");

int main() 
{

	Parameters p (20.0, 0.1, 0.1, 1000);
	Simulator sim (std::make_shared<Parameters>(p));

	sim.run();

	sim.print(1);

	return 0;
}

