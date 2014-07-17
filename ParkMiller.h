// use the park miller implementation from c++11

#ifndef PARK_MILLER_H
#define PARK_MILLER_H

#include "RandomBase.h"
#include <random>
#include <ctime>

// class using the c++11 rand linear_congruential random number gen
class ParkMiller
{
public:
	using random_engine = std::minstd_rand;
	using result_type = random_engine::result_type;
	
	ParkMiller(result_type Seed_=1);
	
	result_type GetOneRandomInteger();
	void SetSeed(result_type Seed);
	void Skip(result_type numberToSkip=1);
	
	static constexpr result_type Min() { return random_engine::min(); }
	static constexpr result_type Max() { return random_engine::max(); }
	
private:
	result_type Seed;
	random_engine generator;
};

class RandomParkMiller : public RandomBase 
{
public:
	using size_type = std::size_t;
	
	RandomParkMiller(value_type NumbersToGenerate_,
			 value_type Seed=1);

	RandomParkMiller* clone() const override { return new RandomParkMiller(*this); }
	// FIXME
	RandomParkMiller* new_random() const override { return new RandomParkMiller(1); }
	
	virtual void GetUniforms(Array& variates) override;
// 	void GetUniforms(boost::python::list &  ns);	
	virtual void SetSeed(value_type Seed) override;
	virtual void Skip(value_type NumberOfPaths=1) override;
	virtual void Reset() override;
	
private:
	ParkMiller InnerGenerator;
	value_type InitialSeed;
	// constexpr?
	const double Reciprocal;
};

#ifdef USE_C_INTERFACE
extern "C"
{
	RandomParkMiller * new_RandomParkMiller(unsigned long l1)
	{
		return new RandomParkMiller(l1, std::time( NULL ));
	}
	
	void RandomParkMiller_SetSeed(RandomParkMiller * rw, unsigned long l1)
	{
		rw->SetSeed(l1);
	}
}
#endif

#endif