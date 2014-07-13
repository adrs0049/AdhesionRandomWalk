// Statistics.cpp

#include "Statistics.h"

StatisticsMean::StatisticsMean()
: RunningSum(0.0), 
RunningSumSquare(0.0),
PathsDone(0UL)
{}

StatisticsMean* StatisticsMean::clone() const
{
	return new StatisticsMean(*this);
}

void StatisticsMean::DumpOneResult(const float_type result)
{
	PathsDone++;
	RunningSum+=result;
	RunningSumSquare+=(result*result);
}

/*
 * GetResultsSoFar returns a vector<vector>
 * At position (0,0) the mean is returned
 * At position (0,1) the variance is returned
 * 
 */
std::vector< std::vector< StatisticsBase::float_type > > StatisticsMean::GetResultsSoFar() const
{
	float_type xaverage = RunningSum / PathsDone;
	float_type x2average = RunningSumSquare / PathsDone;
	
	std::vector<std::vector<float_type>> Results(1);
	Results[0].resize(2);
	Results[0][0] = xaverage;
	Results[0][1] = x2average - xaverage * xaverage;
	
	return Results;
}
