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
	RandomWalkBase() {}
	
	RandomWalkBase(const Wrapper<RandomWalk> theRandomWalk)
	: TheRandomWalk(theRandomWalk)
	{}
	
	virtual void GetOnePath(iArray& values) = 0;
	void DoSimulation(StatisticsBase& TheGatherer, unsigned long NumberOfPaths);
	virtual ~RandomWalkBase(){}
	std::unique_ptr<DataBase> DoOnePath(const iArray& values) const;

private:
	Wrapper<RandomWalk> TheRandomWalk;
};

class SimpleRandomWalk : public RandomWalkBase
{
public:
	SimpleRandomWalk(const Wrapper<RandomBase>& TheGenerator_, 
					 const Wrapper<RandomWalk>& TheRandomWalk_);
	
	virtual void GetOnePath(iArray& values);
	virtual ~SimpleRandomWalk() {}
	
private:
	Wrapper<RandomBase> TheGenerator;
	fArray variates;
};

#endif