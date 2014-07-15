// RandomWalkStatistics.cpp

#include "RandomWalkStatistics.h"

void RandomWalkStatistics::DumpOneResult(std::unique_ptr<DataBase> d)
{
	data.emplace_back(std::move(d));
}

// FIXME
void RandomWalkStatistics::GetResultsSoFar() const
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

std::size_t RandomWalkStatistics::GetElementSize() const
{
	std::size_t sz {0UL};
	for (const auto& elem : data)
		if (sz < elem->size())
			sz = elem->size();

	return sz;
}