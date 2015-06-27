#ifndef PERIODIC_BOUNDARY_H
#define PERIODIC_BOUNDARY_H

#include "Boundary.h"

namespace boundary {

class PeriodicBoundary : public Boundary {

public:
	bool applyCondition(int& coordinate, const int& max_value);

};

} // end namespace

#endif
