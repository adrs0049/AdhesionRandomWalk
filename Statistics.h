// Statistics.h

#ifndef STATISTICS_H
#define STATISTICS_H

#include <valarray>
#include <wrapper.h>
#include <type_traits>
#include <debug.h>
#include <cassert>

#define ARRAY(x) std::valarray<double> {x}

template<class T>
class StatisticsBase
{
public:
	using float_type = double;
	
	StatisticsBase() {}
	
	virtual void DumpOneResult(const std::valarray<T>& result) = 0;
	virtual void DumpOneResult(const std::valarray<T>&& result) = 0;
	virtual std::valarray<std::valarray<float_type> > GetResultsSoFar() const = 0;
	virtual StatisticsBase* clone() const = 0;
	virtual ~StatisticsBase() {}
private:
};

template<class T>
class StatisticsMeanImpl : public StatisticsBase<T>
{
public:
	StatisticsMeanImpl();
	
	virtual void DumpOneResult(const std::valarray<T>& result);
	virtual void DumpOneResult(const std::valarray<T>&& result);
	virtual std::valarray<std::valarray<double> > GetResultsSoFar() const;
	virtual StatisticsMeanImpl* clone() const;
	virtual ~StatisticsMeanImpl() {}
private:
	T RunningSum;
	T RunningSumSquare;
	unsigned long PathsDone;
};

template<class T>
class StatisticsRWImpl : public StatisticsBase<T>
{
public:
    StatisticsRWImpl();
    
    virtual void DumpOneResult(const std::valarray<T>& result);
    virtual void DumpOneResult(const std::valarray<T>&& result);
	virtual std::valarray<std::valarray<double>> GetResultsSoFar() const;
	virtual StatisticsRWImpl* clone() const;
	virtual ~StatisticsRWImpl() {}
	
	void resize(unsigned long outer, unsigned long inner);
private:
	std::valarray<std::valarray<T>> results;
	unsigned long PathsDone;
};

template<class T>
StatisticsRWImpl<T>::StatisticsRWImpl()
: results(1), PathsDone(0UL)
{}

template<class T>
void StatisticsRWImpl<T>::resize(long unsigned int outer, long unsigned int inner)
{
	results.resize(outer);
	for (auto& ivec : results)
		ivec.resize(inner);
}

template<class T>
void StatisticsRWImpl<T>::DumpOneResult(const std::valarray< T >& result)
{
	assert(PathsDone<results.size());
 	results[PathsDone] = result;
	PathsDone++;
}

template<class T>
void StatisticsRWImpl<T>::DumpOneResult(const std::valarray< T >&& result)
{
	DEBUG2(results.size());
	assert(PathsDone<results.size());
	results[PathsDone] = result;
	PathsDone++;
}

template<class T>
StatisticsRWImpl<T>* StatisticsRWImpl<T>::clone() const
{
	return new StatisticsRWImpl(*this);
}

template<class T>
std::valarray< std::valarray< double > > StatisticsRWImpl<T>::GetResultsSoFar() const
{
	return results;
}

template<class T>
StatisticsMeanImpl<T>::StatisticsMeanImpl()
: RunningSum(0.0), 
RunningSumSquare(0.0),
PathsDone(0UL)
{}

template<class T>
StatisticsMeanImpl<T>* StatisticsMeanImpl<T>::clone() const
{
	return new StatisticsMeanImpl(*this);
}

template<class T>
void StatisticsMeanImpl<T>::DumpOneResult(const std::valarray<T>& result)
{
	PathsDone++;
	RunningSum+=result[0];
	RunningSumSquare+=(result[0]*result[0]);
}

template<class T>
void StatisticsMeanImpl<T>::DumpOneResult(const std::valarray<T>&& result)
{
	PathsDone++;
	RunningSum+=result[0];
	RunningSumSquare+=(result[0]*result[0]);
}

/*
 * GetResultsSoFar returns a vector<vector>
 * At position (0,0) the mean is returned
 * At position (0,1) the variance is returned
 * 
 */
template<class T>
std::valarray< std::valarray< double > > StatisticsMeanImpl<T>::GetResultsSoFar() const
{
	double xaverage = RunningSum / PathsDone;
	double x2average = RunningSumSquare / PathsDone;
	
	std::valarray<std::valarray<double>> Results(1);
	Results[0].resize(2);
	Results[0][0] = xaverage;
	Results[0][1] = x2average - xaverage * xaverage;
	
	return Results; 
}

// usefule alias
using StatisticsMean = StatisticsMeanImpl<double>;
using StatisticsRW = StatisticsRWImpl<double>;

#endif