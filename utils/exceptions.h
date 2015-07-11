
#pragma once

#include <exception>
#include <stdexcept>

class NotImplementedException : public std::logic_error
{	public:
		using std::logic_error::logic_error;

		virtual const char * what() const throw()
			{ return "Function not yet implemented."; }
};
