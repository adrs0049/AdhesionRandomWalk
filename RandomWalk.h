// RandomWalk.h

#ifndef RANDOMWALK_H
#define RANDOMWALK_H

#include "array.h"

class RandomWalk
{
public:
	using size_type = std::size_t;
	using array_type = std::vector<unsigned long>;
	
	RandomWalk()
	: LookAtTimes(0), NumberOfSteps(0UL), left(0.5F), right(0.5F)
	{}
	
	RandomWalk(const array_type & a, const unsigned long steps)
	: LookAtTimes(a), NumberOfSteps(steps), left(0.5F), right(0.5F)
	{
		insert_zero();
	}
	
	RandomWalk(array_type&& a, const unsigned long steps)
	: LookAtTimes(a), NumberOfSteps(steps), left(0.5F), right(0.5F)
	{
		a.resize(0);
		insert_zero();
	}
	
	size_type GetLookAtTimesSize(void) const 
	{ return LookAtTimes.size(); };
	
	const array_type& GetLookAtTimes(void) const
	{ return LookAtTimes; }
	
	unsigned long GetNumberOfSteps(void) const
	{ return NumberOfSteps; }
	
	// probabilities
	float LP(void) const { return left; }
	float RP(void) const { return left + right; }
	
	RandomWalk* clone() const { return new RandomWalk(*this); }
	
	~RandomWalk() {}
	
private:
	
	// LookAtTimes must start with a 0
	void insert_zero()
	{
		if (LookAtTimes[0] != 0) 
			LookAtTimes.insert(begin(LookAtTimes), 0UL);
		
		if (LookAtTimes[LookAtTimes.size()-1] != NumberOfSteps+1)
			LookAtTimes.push_back(NumberOfSteps);
	}
	
	// LookAtTimes is saved in terms of # of steps
	array_type LookAtTimes;
	unsigned long NumberOfSteps;
	
	// probabilities
	const float left;
	const float right;
};

// C Interface for python usage
#ifdef USE_C_INTERFACE
extern "C"
{
	RandomWalk * new_RandomWalk(unsigned long * array, unsigned long array_sz, 
								unsigned long steps)
	{
		std::vector<unsigned long> tmp {array, array + array_sz};
		return new RandomWalk(std::move(tmp), steps);
	}
	
	std::size_t RandomWalk_getLookAtTimeSize(RandomWalk * rw)
	{
		return rw->GetLookAtTimesSize();
	}
	
	/* ATTENTION HOW TO DO THIS???
	void * RandomWalk_getLookAtTime(RandomWalk * rw)
	{
		return (void *)rw->GetLookAtTimes();
	}
	*/
	
	unsigned long RandomWalk_GetNumberOfSteps(RandomWalk * rw)
	{
		return rw->GetNumberOfSteps();
	}
}
#endif

#endif