// DataBase.h

#ifndef DATA_BASE_H
#define DATA_BASE_H

#include <cstddef>

struct DataBase
{
	DataBase() {}
	~DataBase() {}
	
	virtual void print(void) const = 0;
	virtual void * getData(void) const = 0;
	virtual std::size_t size(void) const = 0;
};

#endif