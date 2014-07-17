// RandomWalkStatistics.h

#ifndef RANDOM_WALK_STATISTICS_H
#define RANDOM_WALK_STATISTICS_H

#include "StatisticsBase.h"
#include "RandomWalkData.h"

#include <vector>
#include <memory>
#include <iostream>

class RandomWalkStatistics : public StatisticsBase
{
public:
    RandomWalkStatistics()
	: data(0)
	{}
	
	~RandomWalkStatistics() {}
	
	// FIXME
	virtual void DumpOneResult(std::unique_ptr<DataBase> d);
	virtual void PrintResultsSoFar() const;
	virtual DataBase * GetResultsSoFar() const;
	virtual DataBase * GetResultsAtPos(const unsigned long pos) const;
	
	// size functions
	virtual std::size_t GetSize() const;
	virtual std::size_t GetElementSize(const unsigned long pos) const;
	virtual std::size_t GetElementSizeMax() const;
private:
	std::vector<std::unique_ptr<DataBase> > data;
};

#ifdef USE_C_INTERFACE
// C Interface for python usage
extern "C"
{
	RandomWalkStatistics* new_RandomWalkStatistics()
	{
 		return new RandomWalkStatistics();
	}

	void * RandomWalkStatistics_getData(RandomWalkStatistics* stat, 
										const unsigned long pos)
	{
		return stat->GetResultsAtPos(pos);
	}
	
	void RandomWalkStatistics_printData(RandomWalkStatistics* stat)
	{
		stat->PrintResultsSoFar();
	}

	std::size_t RandomWalkStatistics_getSize(RandomWalkStatistics * stat)
	{
		return stat->GetSize();
	}
	
	// get vector element size at a location
	std::size_t RandomWalkStatistics_getElementSize(RandomWalkStatistics * stat, 
													const unsigned long pos)
	{
		return stat->GetElementSize(pos);
	}
	
	// get vector element size at a location
	std::size_t RandomWalkStatistics_getElementSizeMax(RandomWalkStatistics * stat)
	{
		return stat->GetElementSizeMax();
	}
}
#endif

#endif