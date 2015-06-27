#ifndef BOUNDARY_FACTORY_H
#define BOUNDARY_FACTORY_H

#include <string>

#include "Boundary.h"
#include "PeriodicBoundary.h"
#include <cassert>

namespace boundary {

class BoundaryFactory {

public:
	static const std::string no_flux;
	static const std::string periodic;
	//static const std::string dirichlet;

	static Boundary * createBoundary(std::string boundary)
	{
		if (boundary == periodic)
			return new PeriodicBoundary();
		else
			assert(0);
	}


};


//const std::string BoundaryFactory::no_flux("NoFlux");
//const std::string BoundaryFactory::periodic("Periodic");

} // end namespace

#endif
