
#pragma once

#include <vector>
#include <algorithm>

template <typename T, typename A>
std::vector<T, A> operator+(const std::vector<T, A>& a,
							const std::vector<T, A>& b)
{
	assert(a.size() == b.size());

	std::vector<T, A> result;
	result.reserve(a.size());

	std::transform(a.begin(), a.end(), b.begin(),
			std::back_inserter(result), std::plus<T>());
	return result;
}

template <typename T, typename A>
std::vector<T, A> operator-(const std::vector<T, A>& a,
							const std::vector<T, A>& b)
{
	assert(a.size() == b.size());

	std::vector<T, A> result;
	result.reserve(a.size());

	std::transform(a.begin(), a.end(), b.begin(),
			std::back_inserter(result), std::minus<T>());
	return result;
}

template <typename T, typename A>
std::vector<T, A> operator*(const std::vector<T, A>& a,
							const std::vector<T, A>& b)
{
	assert(a.size() == b.size());

	std::vector<T, A> result;
	result.reserve(a.size());

	std::transform(a.begin(), a.end(), b.begin(),
			std::back_inserter(result), std::multiplies<T>());
	return result;
}

template <typename T, typename A>
std::vector<T, A> operator/(const std::vector<T, A>& a,
							const std::vector<T, A>& b)
{
	assert(a.size() == b.size());

	std::vector<T, A> result;
	result.reserve(a.size());

	std::transform(a.begin(), a.end(), b.begin(),
			std::back_inserter(result), std::divides<T>());
	return result;
}


