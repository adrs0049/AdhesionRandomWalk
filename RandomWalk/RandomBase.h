// RandomBase.h
#ifndef RANDOM_BASE_H
#define RANDOM_BASE_H

// includes
#include "array.h"

class RandomBase
{
public:
	using value_type = unsigned long;
	
	RandomBase(value_type NumbersToGenerate_);
	value_type GetNumbersToGenerate(void) { return NumbersToGenerate; }
	
	// virtual constructors
	virtual RandomBase* clone() const = 0;
 	virtual RandomBase* new_random() const = 0;
	
	// pure virtual methods 
	// NOTE variate is an outcome of a random variable
	virtual void GetUniforms(Array& variates) = 0;
	virtual void SetSeed(value_type Seed) = 0;
	virtual void Skip(value_type NumberOfPaths)=0;
	virtual void Reset()=0;
	
	// virtual methods
	virtual void ResetNumbersToGenerate(value_type NewNumbersToGenerate);
	virtual void GetGaussian(Array& variates);
	
private:
	// random numbers to generate per call to function
	value_type NumbersToGenerate;
};

#endif