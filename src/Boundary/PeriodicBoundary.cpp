#include "Boundary.h"
#include "PeriodicBoundary.h"

namespace boundary {

bool PeriodicBoundary::applyCondition(int& coordinate,
									  const int& max_value)
{
	short val;
	if (coordinate < 0)
	{
		val = coordinate % max_value;
		coordinate = (val != 0) ? max_value + val : 0;
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

