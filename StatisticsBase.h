// StatisticsBase.h

#ifndef STATISTICS_BASE_H
#define STATISTICS_BASE_H

#include <memory>

#include "DataBase.h"

class StatisticsBase
{
public:
	StatisticsBase() {}
	~StatisticsBase() {}
	
	virtual void DumpOneResult(std::unique_ptr<DataBase> data) = 0;
	virtual void PrintResultsSoFar() const = 0;
	virtual DataBase * GetResultsSoFar() const = 0;
	virtual DataBase * GetResultsAtPos(const unsigned long pos) const = 0;
	
	// python interfaces need to know how data is formatted
	virtual std::size_t GetSize() const = 0;
	// we may store up to x element per call of a Stat Gatherer
	virtual std::size_t GetElementSize(const unsigned long pos) const = 0;
	// get max element may be needed by python interfaces
	virtual std::size_t GetElementSizeMax() const = 0;
};

#endif