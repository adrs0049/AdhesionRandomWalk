// RandomWalkBase.cpp

#include "RandomWalkBase.h"

void RandomWalkBase::DoSimulation(StatisticsBase& TheGatherer, 
								  unsigned long NumberOfPaths)
{
	// hold a single path
	iArray PathValues(TheRandomWalk->GetNumberOfSteps());

	for (unsigned long path = 0; path < NumberOfPaths; ++path)
	{
		GetOnePath(PathValues);
 		TheGatherer.DumpOneResult(DoOnePath(PathValues));
	}
}

std::unique_ptr<DataBase> RandomWalkBase::DoOnePath(const iArray& values) const
{
	result_type results; // (TheRandomWalk->GetLookAtTimes());
	std::vector<unsigned long> LookAtTimes = TheRandomWalk->GetLookAtTimes();

	long xaverage {0L};
	unsigned long x2average {0UL};
		
	for (unsigned long i = 0; i < TheRandomWalk->GetLookAtTimesSize()-1; ++i)
	{	
		for (unsigned long j = LookAtTimes[i]; j < LookAtTimes[i+1]; ++j)
		{
			xaverage += values[j];
			x2average += values[j] * values[j];
		}
		results.push_back(std::make_pair(xaverage, x2average));
	}
	
	std::unique_ptr<DataBase> TheResultPtr {new RandomWalkData(std::move(results))};
	return TheResultPtr;
}

SimpleRandomWalk::SimpleRandomWalk(const Wrapper<RandomBase>& TheGenerator_,
								   const Wrapper<const RandomWalk>& TheRandomWalk_)
: RandomWalkBase(TheRandomWalk_), 
  TheGenerator(TheGenerator_), 
 variates(TheRandomWalk_->GetNumberOfSteps())
{
	TheGenerator->ResetNumbersToGenerate(TheRandomWalk_->GetNumberOfSteps());
}

void SimpleRandomWalk::GetOnePath(iArray& values)
{
	assert(values.size() == variates.size());
	TheGenerator->GetUniforms(variates);
	
	auto iter = begin(values);
	for (const auto& elem : variates) 
	{
		if (elem <= TheRandomWalk->LP())
			*iter=1L;
		else if (elem <= TheRandomWalk->RP())
			*iter=-1L;
		else // if neither above are true we don't move
			*iter=0L;
		++iter;	
	}
}
