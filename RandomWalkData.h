// RandomWalkData.h

#ifndef RANDOM_WALK_DATA_H
#define RANDOM_WALK_DATA_H

#include "DataBase.h"
#include <cstddef>
#include <utility>
#include <vector>
#include <iostream>
#include <iomanip>
#include <string>

// the first is position average can be negative
// the second is the variance in the position ie square sum -> positive
using result_type = std::vector<std::pair<long, unsigned long>>;
// constexpr std::size_t NumberOfElementsPerVector {2UL};
constexpr unsigned int _RandomWalkData_field_width_ {2U};

#define PRINT_RESULT(x) { \
	std::string str {#x}; \
	std::cout << std::setiosflags(std::ios::showpoint | std::ios::uppercase); \
	std::cout << std::setw(str.length()) << #x << ": ["; \
	for (const auto elem : x) \
		std::cout << "{ " << std::setw(_RandomWalkData_field_width_) << \
		std::setprecision(6) << elem.first << " ," << \
		std::setw(_RandomWalkData_field_width_) << elem.second << "}"; \
	std::cout << "]" << std::endl; \
}

struct RandomWalkData : public DataBase
{
    RandomWalkData()
	: data(0)
	{}
	
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
		if (size() == 0)
			return;
		
		constexpr unsigned int field_width = 2;
		for (const auto elem : data) {
			std::cout << "{" << std::setw(field_width) << elem.first << ',' \
					  << std::setw(field_width) << elem.second << "}, ";
		}
 		std::cout << std::endl;
	}
	
	virtual void * getData() const
	{
		return (void*)(data.data());
	}
	
	virtual std::size_t size() const
	{
		return  data.size();
	}
	
#ifdef DEBUG_BUILD
	// function for DEBUG
	void AddData(std::pair<long, unsigned long>&& p)
	{
		data.emplace_back(p);
	}
#endif
	
	// DATA
	result_type data;
};

#ifdef USE_C_INTERFACE
// C interface for use with python
extern "C"
{
	RandomWalkData* new_RandomWalkData()
	{
		return new RandomWalkData();
	}
	
#ifdef DEBUG_BUILD
	void RandomWalkData_addData(RandomWalkData* data, long l1, unsigned long l2)
	{
		data->AddData(std::make_pair(l1, l2));
	}
#endif
	
	void RandomWalkData_print(RandomWalkData* data)
	{
		data->print();
	}
	
	std::size_t RandomWalkData_getSize(RandomWalkData* data)
	{
		return data->size();
	}

	void * RandomWalkData_getData(RandomWalkData* data)
	{
		return data->getData();
	}
}
#endif

#endif