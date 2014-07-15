// RandomWalk.h

#ifndef RANDOMWALK_H
#define RANDOMWALK_H

#include "array.h"
#include <debug.h>

class RandomWalk
{
public:
	using size_type = std::size_t;
	using array_type = std::vector<unsigned long>;
	
	RandomWalk() {}
	
	RandomWalk(const array_type & a, const unsigned long steps)
	: LookAtTimes(a), NumberOfSteps(steps)
	{
		insert_zero();
	}
	
	RandomWalk(array_type&& a, const unsigned long steps)
	: LookAtTimes(a), NumberOfSteps(steps)
	{
		a.resize(0);
		insert_zero();
	}
	
	const size_type GetLookAtTimesSize(void) const 
	{ return LookAtTimes.size(); };
	
	const array_type& GetLookAtTimes(void) const
	{ return LookAtTimes; }
	
	const unsigned long GetNumberOfSteps(void) const
	{ return NumberOfSteps; }
	
	RandomWalk* clone() const { return new RandomWalk(*this); }
	
	~RandomWalk() {}
	
private:
	
	// LookAtTimes must start with a 0
	void insert_zero()
	{
		if (LookAtTimes[0] != 0) 
			LookAtTimes.insert(begin(LookAtTimes), 0UL);
	}
	
	// LookAtTimes is saved in terms of # of steps
	array_type LookAtTimes;
	unsigned long NumberOfSteps;
};

#endif