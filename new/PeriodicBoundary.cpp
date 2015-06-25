#include "Boundary.h"
#include "PeriodicBoundary.h"

#include <cmath>

#include <iostream>

namespace boundary {

bool PeriodicBoundary::applyCondition(int& coordinate, const int& max_value)
{
        short val;

	if (coordinate < 0) 
	{
		val = std::abs((float)(coordinate % max_value));
                std::cout << "coordinate="<<coordinate << " max=" << max_value << " val = "<<val<<std::endl;
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

