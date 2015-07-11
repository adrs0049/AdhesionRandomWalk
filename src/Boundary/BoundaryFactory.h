#ifndef BOUNDARY_FACTORY_H
#define BOUNDARY_FACTORY_H

#include <string>

#include "Boundary.h"
#include "PeriodicBoundary.h"
#include "exceptions.h"

namespace boundary {

class BoundaryFactory {

public:
	static Boundary * createBoundary(const BOUNDARY_TYPE& boundary)
	{
        switch (boundary)
        {
            case BOUNDARY_TYPE::PERIODIC:
                return new PeriodicBoundary();
                break;
            case BOUNDARY_TYPE::DIRICHLET:
            case BOUNDARY_TYPE::NEUMANN:
            default:
                throw NotImplementedException {""};
        };
	    return nullptr;
    }
};

} // end namespace

#endif
