#include "log.h"
#include <map>

namespace Log {

const std::map<log_level_t, std::string> formatted_log_t::Log_Map = {
	{LOG::NOTHING, ""},
	{LOG::CRITICAL, "Critical: "},
	{LOG::ERROR, "Error: "},
	{LOG::WARNING, "Warning: "},
	{LOG::INFO, "Info: "},
	{LOG::DEBUG, "Debug: "},
};


} // end namespace
