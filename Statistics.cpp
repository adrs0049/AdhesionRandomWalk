// Statistics.cpp

#include "Statistics.h"

StatisticsMean::StatisticsMean()
: RunningSum(0.0), PathsDone(0)
{}

StatisticsMean* StatisticsMean::clone() const
{
	return new StatisticsMean(*this);
}

void StatisticsMean::DumpOneResult(double result)
{
	PathsDone++;
	RunningSum+=result;
}

std::vector< std::vector< double > > StatisticsMean::GetResultsSoFar() const
{
	std::vector<std::vector<double>> Results(1);
	Results[0].resize(1);
	Results[0][0] = RunningSum / PathsDone;
	
	return Results;
}
