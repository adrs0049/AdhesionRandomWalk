// Statistics.h

#ifndef STATISTICS_H
#define STATISTICS_H

#include "array.h"
#include <type_traits>

class StatisticsBase
{
public:
	using float_type = double;
	
	StatisticsBase() {}
	
	virtual void DumpOneResult(const float_type result) = 0;
	virtual std::vector<std::vector<float_type> > GetResultsSoFar() const = 0;
	virtual StatisticsBase* clone() const = 0;
	virtual ~StatisticsBase() {}
private:
};

class StatisticsMean : public StatisticsBase
{
public:
	StatisticsMean();
	
	virtual void DumpOneResult(const float_type result);
	virtual std::vector<std::vector<float_type> > GetResultsSoFar() const;
	virtual StatisticsMean* clone() const;
	virtual ~StatisticsMean() {}
private:
	float_type RunningSum;
	float_type RunningSumSquare;
	unsigned long PathsDone;
};

#endif