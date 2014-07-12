// Statistics.h

#ifndef STATISTICS_H
#define STATISTICS_H

#include "array.h"

class StatisticsBase
{
public:
	StatisticsBase() {}
	
	virtual void DumpOneResult(double result) = 0;
	virtual std::vector<std::vector<double> > GetResultsSoFar() const = 0;
	virtual StatisticsBase* clone() const = 0;
	virtual ~StatisticsBase() {}
private:
};

class StatisticsMean : public StatisticsBase
{
public:
	StatisticsMean();
	
	virtual void DumpOneResult(double result);
	virtual std::vector<std::vector<double> > GetResultsSoFar() const;
	virtual StatisticsMean* clone() const;
	virtual ~StatisticsMean() {}
private:
	double RunningSum;
	unsigned long PathsDone;
};

#endif