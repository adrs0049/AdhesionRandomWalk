// RandomWalkBase.cpp

#include "RandomWalkBase.h"

void RandomWalkBase::DoSimulation(StatisticsBase& TheGatherer, 
								  unsigned long NumberOfPaths)
{
	DEBUG("DoSim begin\n");
	// hold a single path
	DEBUG2(TheRandomWalk->GetNumberOfSteps());
	
	iArray PathValues(TheRandomWalk->GetNumberOfSteps());

	for (unsigned long path = 0; path < NumberOfPaths; ++path)
	{
		DEBUG2(path);
		GetOnePath(PathValues);
		PRINT_VAL(PathValues);
// 		auto TheResult = ;
 		TheGatherer.DumpOneResult(DoOnePath(PathValues));
	}
	
	DEBUG("DoSim done\n");
	
}

std::unique_ptr<DataBase> RandomWalkBase::DoOnePath(const iArray& values) const
{
	DEBUG("DoOnePATH\n");

	result_type results; // (TheRandomWalk->GetLookAtTimes());
// 	results.resize(TheRandomWalk->GetLookAtTimesSize());
	std::vector<unsigned long> LookAtTimes = TheRandomWalk->GetLookAtTimes();
	
	DEBUG2(results.size());
	DEBUG2(TheRandomWalk->GetLookAtTimesSize());
	DEBUG2(values.size());
	
	PRINT_VAL(LookAtTimes);
	
	long xaverage {0L};
	unsigned long x2average {0UL};
		
	for (unsigned long i = 0; i < TheRandomWalk->GetLookAtTimesSize()-1; ++i)
	{	
		DEBUG2(LookAtTimes[i]);
		DEBUG2(LookAtTimes[i+1]);
		assert(LookAtTimes[i] < values.size());
		assert(LookAtTimes[i+1] <= values.size());
		for (unsigned long j = LookAtTimes[i]; j < LookAtTimes[i+1]; ++j)
		{
			assert(j <= values.size());
			DEBUG2(j);
			xaverage += values[j];
			x2average += values[j] * values[j];
		}
		
		DEBUG2(xaverage);
		DEBUG2(x2average);
		results.push_back(std::make_pair(xaverage, x2average));
	}
	
	DEBUG("before moving to unique_ptr\n");
	PRINT_RESULT(results);
	
	std::unique_ptr<DataBase> TheResultPtr {new RandomWalkData(std::move(results))};
	
	DEBUG("Done DoOnePath\n");
	return TheResultPtr;
}

SimpleRandomWalk::SimpleRandomWalk(const Wrapper<RandomBase>& TheGenerator_,
								   const Wrapper<RandomWalk>& TheRandomWalk_)
: RandomWalkBase(TheRandomWalk_), 
  TheGenerator(TheGenerator_), 
 variates(TheRandomWalk_->GetNumberOfSteps())
{
	TheGenerator->ResetNumbersToGenerate(TheRandomWalk_->GetNumberOfSteps());
}

void SimpleRandomWalk::GetOnePath(iArray& values)
{
	DEBUG("start GetonePath\n");
	TheGenerator->GetUniforms(variates);
	
	for (unsigned long i = 0; i < variates.size(); ++i)
	{
		if (variates[i] <= 0.5)
			values[i]=1L;
		else
			values[i]=-1L;
	}
// 	PRINT_VAL(variates);
	DEBUG("end GetonePath\n");
}
