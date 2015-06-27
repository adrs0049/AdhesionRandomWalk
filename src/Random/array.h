// array.h

#ifndef ARRAY_H
#define ARRAY_H

#include <iostream>
#include <iomanip>
#include <string>
#include <ios>

#include <valarray>
typedef std::valarray<double> Array;
typedef std::valarray<double> fArray;
typedef std::valarray<long> iArray;

constexpr unsigned int a_field_width {3U};

#define PRINT_VAL(x) { \
	std::string str {#x}; \
	std::cout << std::setiosflags(std::ios::showpoint | std::ios::uppercase); \
	std::cout << std::setw(str.length()) << #x << ": ["; \
	for (unsigned long i = 0; i < x.size(); ++i) \
		std::cout << std::setw(a_field_width) << std::setprecision(6) << x[i];  \
	std::cout << "]" << std::endl;		\
}

#endif