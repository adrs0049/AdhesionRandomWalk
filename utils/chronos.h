/*
 * chronos.h
 *
 * Andreas Buttenschoen
 * 2014
 *
 */

#ifndef CHRONOS_H
#define CHRONOS_H

#define CHRONOS_H_VERSION 0.1

#include <chrono>
#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>

using std::chrono::high_resolution_clock;
using std::chrono::duration_cast;
using std::chrono::hours;
using std::chrono::minutes;
using std::chrono::seconds;
using std::chrono::milliseconds;
using std::chrono::microseconds;
using std::chrono::nanoseconds;

namespace chronos {

namespace detail {

enum class UNIT : unsigned short { HR, MIN, S, MS, US, NS };

std::string get_unit(UNIT unit)
{
	switch (unit) {
        case UNIT::HR:
            return {" (hr)"};
        case UNIT::MIN:
            return {" (min)"};
		case UNIT::S:
			return {" (s)"};
		case UNIT::MS:
			return {" (ms)"};
		case UNIT::US:
			return {" (us)"};
		case UNIT::NS:
			return {" (ns)"};
	}
	// shut up compiler
	return {""};
}

} // end namespace

class Chronos {
public:
	Chronos(std::ostream & m_os)
	: start(high_resolution_clock::now()), os(m_os)
	{}

	Chronos()
	: Chronos(std::cout)
	{}

	~Chronos()
	{
		auto end = high_resolution_clock::now();

        auto dhours = duration_cast<hours>(end-start);
        auto dmin = duration_cast<minutes>(end-start-dhours);
		auto ds = duration_cast<seconds>(end-start-dmin);
		auto dms = duration_cast<milliseconds>(end-start-ds);
		auto dus = duration_cast<microseconds>(end-start-ds-dms);
		auto dns = duration_cast<nanoseconds>(end-start-ds-dms-dus);

		std::ostringstream ostr;
		detail::UNIT unit;

		ostr << "Program Execution Time: ";
		if (dhours.count()!=0) {
            unit=detail::UNIT::HR;
            ostr << dhours.count() << ":";
            ostr << dmin.count() << ":";
            ostr << ds.count();
        } else if (dmin.count()!=0) {
            unit=detail::UNIT::MIN;
            ostr << dmin.count() << ":";
            ostr << ds.count();
        } else if (ds.count()!=0) {
			unit=detail::UNIT::S;
			ostr << ds.count() << ".";
			ostr << dms.count();
		} else if (dms.count()!=0) {
			unit=detail::UNIT::MS;
			ostr << dms.count() << ".";
			ostr << dus.count();
		} else if (dus.count()!=0) {
			unit=detail::UNIT::US;
			ostr << dus.count() << ".";
	        ostr << dns.count();
        } else {
			unit=detail::UNIT::NS;
		    ostr << dns.count();
        }

        ostr << detail::get_unit(unit);
		ostr << std::endl;

		os << ostr.str();
	}

private:
	high_resolution_clock::time_point start;
	std::ostream & os;
};

} // end namespace

#endif
