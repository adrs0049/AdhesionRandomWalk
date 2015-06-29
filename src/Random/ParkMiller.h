// use the park miller implementation from c++11

#ifndef PARK_MILLER_H
#define PARK_MILLER_H

//#include "RandomNumberGenerator.h"
#include <random>
#include <ctime>

// class using the c++11 rand linear_congruential random number gen
template <typename distribution>
class ParkMiller
{
public:
	using random_engine = std::mt19937_64;
	using result_type = typename distribution::result_type;
	using engine_result_type = random_engine::result_type;
	using param_type = typename distribution::param_type;

	ParkMiller()
	: Seed(0), generator(), dist(0.0, 1.0)
	{
		init();
	}

	ParkMiller(unsigned long seed)
	: Seed(seed), generator(), dist(0.0, 1.0)
	{
		init();
	}

	ParkMiller(const param_type& param, engine_result_type Seed_=1)
	: Seed(Seed_), generator(), dist(param)
	{
		init();
	}

	ParkMiller(const result_type a, const result_type b)
	: Seed(0), generator(), dist(a, b)
	{
		init();
	}

	result_type GetOneRandomInteger()
	{
		return dist(generator);
	}

	result_type operator() () { return GetOneRandomInteger(); }

	void SetSeed(engine_result_type Seed_)
	{
		Seed = Seed_;
		if (Seed==0) Seed = 1;
		generator.seed(Seed);
	}

	void Skip(engine_result_type numberToSkip=1)
	{
		generator.discard(numberToSkip);
	}

	void Reset()
	{
		dist.reset();
	}


	static constexpr result_type Min() { return distribution::min(); }
	static constexpr result_type Max() { return distribution::max(); }

private:
	void init()
	{
		if (Seed == 0) generator.seed(rd());
		else generator.seed(Seed);
	}

	result_type Seed;
	random_engine generator;
	distribution dist;

	std::random_device rd;
};

using UniformRandomNumberGenerator = ParkMiller<std::uniform_real_distribution<double> >;
using UniformDiscreteRandomNumberGenerator = ParkMiller<std::uniform_int_distribution<unsigned long> >;

#endif
