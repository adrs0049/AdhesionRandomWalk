// RandomBase.cpp

#include "RandomBase.h"

RandomBase::RandomBase(RandomBase::value_type NumbersToGenerate_)
: NumbersToGenerate(NumbersToGenerate_)
{}

void RandomBase::ResetNumbersToGenerate(RandomBase::value_type NewNumbersToGenerate)
{
	NumbersToGenerate = NewNumbersToGenerate;
}

void RandomBase::GetGaussian(Array& variates)
{
	// FIXME
}
