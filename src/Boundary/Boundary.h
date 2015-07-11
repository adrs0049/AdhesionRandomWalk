#ifndef BOUNDARY_H
#define BOUNDARY_H

namespace boundary {

enum class BOUNDARY_TYPE : int
{
	PERIODIC,
	NEUMANN,
	DIRICHLET,
};

class Boundary {

public:
	virtual bool applyCondition(int& coordinate, const int& max_value)=0;
};

} // boundary

#endif

