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
	virtual void GetResultsSoFar() const;
	
	// size functions
	virtual std::size_t GetSize() const;
	virtual std::size_t GetElementSize() const;
	
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
	
	void RandomWalkStatistics_getData(RandomWalkStatistics* stat)
	{
		stat->GetResultsSoFar();
	}
	
	// ATTENTION NO IDEA IF THIS WORKS AND IS NEEDED
	/*
	void RandomWalkStatistics_DumpData(RandomWalkStatistics * stat, DataBase * d)
	{
		stat->DumpOneResult(std::unique_ptr<DataBase>(nullptr));
	}
	*/
	
	std::size_t RandomWalkStatistics_getSize(RandomWalkStatistics * stat)
	{
		return stat->GetSize();
	}
	
	std::size_t RandomWalkStatistics_getElementSize(RandomWalkStatistics * stat)
	{
		return stat->GetElementSize();
	}
}
#endif

#endif