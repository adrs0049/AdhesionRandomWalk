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

#endif