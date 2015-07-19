#include <iostream>
#include "Simulator.h"
#include <memory>
#include <vector>
#include <string>
#include <sstream>
#include <chronos.h>

#include "Parameters.h"

int main(int argc, const char* argv[])
{
	long NumberOfCells = 10;
	if (argc==2)
	{
		std::istringstream ss(argv[1]);
		if(!(ss>>NumberOfCells))
			std::cerr << "Invalid Number " << argv[1] << std::endl;
	}
	else if (argc>2)
		exit(0);

	std::cout << "Simulation running with " << NumberOfCells << " of cells." << std::endl;

	//std::vector<double> FinalTimes {0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9};
	std::vector<double> FinalTimes {1.0};

	for (auto& time : FinalTimes)
	{
		std::cout << "Running simulation up to " << time << "." << std::endl;

		chronos::Chronos timer;

		Parameters p (10.0, 20, FinalTimes, NumberOfCells);
		p.setDiffusion(1.0);
		p.setDrift(1.0);
		p.setSensingRadius(1.0);
		p.setIcType(IC_TYPE::UNIFORM);
		p.setRandomWalkType(RANDOMWALK_TYPE::ADHESION);

		Simulator sim (std::make_shared<Parameters>(p));

		sim.run();
	}

	return 0;
}

