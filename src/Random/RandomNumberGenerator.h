#ifndef RANDOM_NUMBER_GENERATOR_H
#define RANDOM_NUMBER_GENERATOR_H

class RandomNumberGenerator {

public:

	using value_type = unsigned long;

	virtual void SetSeed(value_type Seed) = 0;
	virtual void Skip(value_type NumberOfPaths = 1) = 0;
	virtual void Reset() = 0;

};

#endif

