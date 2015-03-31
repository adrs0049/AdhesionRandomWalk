#ifndef BOUNDARY_H
#define BOUNDARY_H

namespace boundary {

class Boundary {

public:
	virtual bool applyCondition(int& coordinate, const unsigned int& max_value)=0;
};

} // boundary

#endif

