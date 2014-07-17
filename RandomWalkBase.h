// RandomWalkBase

#ifndef RANDOMWALK_BASE_H
#define RANDOMWALK_BASE_H

#include <wrapper.h>
#include <memory>
#include <utility>
#include <cassert>

#include "array.h"
#include "StatisticsBase.h"
#include "RandomBase.h"
#include "RandomWalk.h"
#include "RandomWalkData.h"

// #define NDEBUG
#include <debug.h>

class RandomWalkBase
{
public:
	// this doesn't make any sense
 	RandomWalkBase() = delete;
	
	RandomWalkBase(const Wrapper<const RandomWalk>& theRandomWalk)
	: TheRandomWalk(theRandomWalk)
	{}
	
	virtual void GetOnePath(iArray& values) = 0;
	void DoSimulation(StatisticsBase& TheGatherer, unsigned long NumberOfPaths);
	virtual ~RandomWalkBase(){}
	std::shared_ptr<DataBase> DoOnePath(const iArray& values) const;

protected:
	const Wrapper<const RandomWalk> TheRandomWalk;
};

class SimpleRandomWalk : public RandomWalkBase
{
public:
	SimpleRandomWalk(const Wrapper<RandomBase>& TheGenerator_, 
					 const Wrapper<const RandomWalk>& TheRandomWalk_);
	
	virtual void GetOnePath(iArray& values);
	virtual ~SimpleRandomWalk() {}
	
private:
	Wrapper<RandomBase> TheGenerator;
	fArray variates;
};

// C interface for use with python
#ifdef USE_C_INTERFACE
extern "C"
{
	SimpleRandomWalk * new_SimpleRandomWalk(RandomBase * g, 
											RandomWalk * rw)
	{
		return new SimpleRandomWalk(g, rw);
	}
	
	void RandomWalk_DoSimulation(RandomWalkBase * rw, StatisticsBase& stat, 
								 unsigned long paths)
	{
		rw->DoSimulation(stat, paths);
	}
	
}
#endif

#endif