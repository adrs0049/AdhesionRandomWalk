#include "Boundary.h"
#include "PeriodicBoundary.h"

#include <cmath>

namespace boundary {

bool PeriodicBoundary::applyCondition(int& coordinate, const unsigned int& max_value)
{
	if (coordinate < 0) 
	{
		auto val = std::abs((float)(coordinate % max_value));
		coordinate = max_value - val;
		return true;
	}
	else if (coordinate >= max_value) 
	{
		coordinate = coordinate % max_value;
		return true;	
	}
	
	return false;
}

} // end namespace

