
#pragma once

#include <exception>
#include <stdexcept>
#include <string>

class NotImplementedException : public std::logic_error
{	public:
		using std::logic_error::logic_error;

		NotImplementedException(const std::string & msg)
			: logic_error("Function " + msg + " not implemented")
		{}
};

class NullPtrDereference : public std::runtime_error
{
	public:
		using std::runtime_error::runtime_error;

		NullPtrDereference(const std::string & msg)
			: runtime_error("Pointer " + msg + " is NULL!")
		{}
};
