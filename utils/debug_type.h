#pragma once

#include <string>
#include <iostream>
#include <iomanip>
#include <typeinfo>
#include <cxxabi.h>

#define DEBUG_TYPE(x) do { typedef void(*T)x; debug_type<T>(T(), #x); } while(0)

template<typename T>
struct debug_type
{
	template<typename U>
	debug_type(void(*)(U), const std::string& p_str)
	{
		std::string str(p_str.begin() + 1, p_str.end() - 1);
		std::cout << str << " => ";
		char * name = nullptr;
		int status;
		name = abi::__cxa_demangle(typeid(U).name(), 0, 0, &status);
		if (name != 0) { std::cout << name << std::endl; }
		else { std::cout << typeid(U).name() << std::endl; }
		free(name);
	}
};

