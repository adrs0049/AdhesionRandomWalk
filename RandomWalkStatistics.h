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
	virtual void DumpOneResult(std::shared_ptr<DataBase> d);
	virtual void PrintResultsSoFar() const;
	virtual DataBase * GetResultsSoFar() const;
	virtual DataBase * GetResultsAtPos(const unsigned long pos) const;
	
	// size functions
	virtual std::size_t GetSize() const;
	virtual std::size_t GetElementSize(const unsigned long pos) const;
	virtual std::size_t GetElementSizeMax() const;
private:
	std::vector<std::shared_ptr<DataBase> > data;
};

#ifdef USE_C_INTERFACE
// C Interface for python usage
extern "C"
{
	RandomWalkStatistics* new_RandomWalkStatistics(double p[][2], std::size_t dim)
	{
		std::vector<std::pair<long, unsigned long>> res;
		for (unsigned long i = 0; i < dim; ++i)
			res.push_back(std::make_pair(p[i][0],p[i][1]));
		
		for (unsigned long i = 0; i < dim; ++i)
			res.push_back(std::make_pair(p[i][0],p[i][1]));
		
		for (unsigned long i = 0; i < dim; ++i)
			res.push_back(std::make_pair(p[i][0],p[i][1]));
		
		auto ptr = new RandomWalkStatistics();
		ptr->DumpOneResult(std::shared_ptr<DataBase> (new RandomWalkData(std::move(res))));
		return ptr;
		
// 		return new RandomWalkStatistics();
	}
	
	RandomWalkStatistics* new2_RandomWalkStatistics()
	{
		std::vector<std::pair<long, unsigned long>> res;
		for (unsigned long i = 0; i < 5; ++i)
			for (unsigned long j = 0; j < 5; ++j)
				res.push_back(std::make_pair(i,j));
		
		auto ptr = new RandomWalkStatistics();
		ptr->DumpOneResult(std::shared_ptr<DataBase> (new RandomWalkData(std::move(res))));
		return ptr;
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