// ParkMiller.cpp

#include "ParkMiller.h"

ParkMiller::ParkMiller(ParkMiller::result_type Seed_)
: Seed(Seed_), generator()
{
	if (Seed==0) Seed = 1;
	generator.seed(Seed);
}

void ParkMiller::SetSeed(ParkMiller::result_type Seed_)
{
	Seed = Seed_;
	if (Seed==0) Seed = 1;
	generator.seed(Seed);
}

void ParkMiller::Skip(result_type numberToSkip)
{
	generator.discard(numberToSkip);
}

ParkMiller::result_type ParkMiller::GetOneRandomInteger()
{
	return generator();
}

RandomParkMiller::RandomParkMiller(RandomBase::value_type NumbersToGenerate_, 
				   RandomBase::value_type Seed)
: RandomBase(NumbersToGenerate_),
InnerGenerator(Seed),
InitialSeed(Seed),
Reciprocal(1/(1.0+InnerGenerator.Max()))
{}

void RandomParkMiller::GetUniforms(Array& variates)
{
	for (size_type i = 0; i < GetNumbersToGenerate(); i++)
		variates[i] = InnerGenerator.GetOneRandomInteger()*Reciprocal;
}

/*
void RandomParkMiller::GetUniforms(boost::python::list& ns)
{
	for (size_type i = 0; i < GetNumbersToGenerate(); ++i)
		ns.append(InnerGenerator.GetOneRandomInteger()*Reciprocal);
}
*/
void RandomParkMiller::Reset()
{
	InnerGenerator.SetSeed(InitialSeed);
}

void RandomParkMiller::SetSeed(RandomBase::value_type Seed)
{
	InitialSeed = Seed;
	InnerGenerator.SetSeed(Seed);
}

void RandomParkMiller::Skip(RandomBase::value_type NumberOfPaths)
{
	InnerGenerator.Skip(NumberOfPaths * GetNumbersToGenerate());
}
