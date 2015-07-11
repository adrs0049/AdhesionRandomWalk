
#pragma once

#include <string>
#include <iostream>
#include <exception>
#include <stdexcept>
#include <sstream>
#include <map>
#include <format.h>

// fix this somehow, can cmake set this value?
#define CURRENT_LEVEL 0

enum class LOG {
	NOTHING,
	CRITICAL,
	ERROR,
	WARNING,
	INFO,
	DEBUG
};

namespace Log {

	using log_level_t = LOG;

	class formatted_log_t
	{
		public:
			formatted_log_t(log_level_t level, const std::string& name)
				: level(level), name(name), w()
			{}

			~formatted_log_t()
			{}

			void report(const char* what)
			{
				w << what;
			}

			template<typename... Args>
			void report(const char* format, const Args&... args)
			{
				w.write(format, args...);
			}

		protected:
			log_level_t		level;
			const char*		fmt;
			std::string		name;
			fmt::MemoryWriter w;

			static const std::map<log_level_t, std::string> Log_Map;
	};

} // end namespace

template<LOG level, typename... Args>
Log::formatted_log_t log(const char* format, const Args&... args)
{
	Log::formatted_log_t logger (level, "default");
	logger.report(format, args...);
}
