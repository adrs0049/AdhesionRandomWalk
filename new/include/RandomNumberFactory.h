#ifndef RANDOM_NUMBER_GENERATOR_FACTORY_H
#define RANDOM_NUMBER_GENERATOR_FACTORY_H

#include <string>

#include "RandomBase.h"
#include "Uniform.h"
#include <cassert>

class RandomNumberGeneratorFactory {

public:
	static const std::string uniform_real;
	static const std::string uniform_discrete;

	static RandomNumberGenerator* createRandomNumberGenerator(std::string generator)
	{
		if (generator == uniform_real)
			return new UniformRealGenerator();
		else
			assert(0);
	}


};

//const std::string BoundaryFactory::no_flux("NoFlux");
//const std::string BoundaryFactory::periodic("Periodic");

#endif
