// RandomWalkStatistics.cpp

#include "RandomWalkStatistics.h"

void RandomWalkStatistics::DumpOneResult(std::shared_ptr<DataBase> d)
{
	data.emplace_back(std::move(d));
}

DataBase * RandomWalkStatistics::GetResultsSoFar() const
{
	for (const auto& elem : data)
	{
		elem->getData();
	}
}

DataBase * RandomWalkStatistics::GetResultsAtPos(const unsigned long pos) const
{
	return data.at(pos).get();
}

// FIXME
void RandomWalkStatistics::PrintResultsSoFar() const
{
	unsigned int run {1U};
	for (const auto& elem : data) {
		std::cout << "Run " << run++ << ": ";
		elem->print();
	}
}

std::size_t RandomWalkStatistics::GetSize() const
{
	return data.size();
}

std::size_t RandomWalkStatistics::GetElementSizeMax() const
{
std::size_t sz {0UL};
	for (const auto& elem : data)
		if (sz < elem->size())
			sz = elem->size();
	return sz;
}

std::size_t RandomWalkStatistics::GetElementSize(const unsigned long pos) const
{
	/*
	std::size_t sz {0UL};
	for (const auto& elem : data)
		if (sz < elem->size())
			sz = elem->size();
	*/
	return data.at(pos)->size();
}