// RandomWalkData.h

#ifndef RANDOM_WALK_DATA_H
#define RANDOM_WALK_DATA_H

#include "DataBase.h"
#include <cstddef>
#include <utility>
#include <vector>
#include <iostream>
#include <iomanip>

using result_type = std::vector<std::pair<unsigned long, long>>;

struct RandomWalkData : public DataBase
{
    RandomWalkData() {}
    RandomWalkData(result_type& result)
	: data(result)
	{}
	
	RandomWalkData(result_type&& result)
	: data(result)
	{
		result.clear();
	}
	
	virtual void print() const
	{
		for (const auto elem : data)
			std::cout << "{" << std::setw(8) << elem.first << ',' \
					  << std::setw(8) << elem.second << "}, ";
		std::cout << std::endl;
	}
	
	virtual void * getData() const
	{
		return (void*)(data.data());
	}
	
	virtual std::size_t size() const
	{
		// each vector elem contains two longs
		return 2*data.size();
	}
	
#ifdef DEBUG_BUILD
	// function for DEBUG
	void AddData(std::pair<unsigned long, long>&& p)
	{
		data.emplace_back(p);
	}
#endif
	
	// DATA
	result_type data;
};

// C interface for use with python
extern "C"
{
	RandomWalkData* new_RandomWalkData()
	{
		return new RandomWalkData();
	}
	
#ifdef DEBUG_BUILD
	void RandomWalkData_addData(RandomWalkData* data, unsigned long l1, long l2)
	{
		data->AddData(make_pair(l1, l2));
	}
#endif
	
	void RandomWalkData_print(RandomWalkData* data)
	{
		data->print();
	}
	
	std::size_t RandomWalkData_size(RandomWalkData* data)
	{
		return data->size();
	}

	void * RandomWalkData_getData(RandomWalkData* data)
	{
		return data->getData();
	}
}

#endif