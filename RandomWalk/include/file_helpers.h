/*
 * file_helpers.h
 * 
 * Some small helper functions for dealing with files
 * 
 * Andreas Buttenschoen 
 * 2014
 * 
 */

#ifndef FILE_HELPERS_H
#define FILE_HELPERS_H

#define FILE_HELPERS_H_VERSION 0.1

#include <debug.h>
#include <ctime>
#include <string>
#include <iostream>
#include <fstream>
#include <ios>
#include <sstream>

#include <boost/filesystem.hpp>
using namespace boost::filesystem;

#define TIME_FORMAT_LENGTH 17

namespace FileHelper {

namespace detail {

static const std::string default_ext {".dat"};
static const std::string default_dir_name {"data"};

std::string TimeString(const std::string format)
{
	// get time
	time_t rawtime;
	time (&rawtime);
	
	// create timeinfo required by strftime
	struct tm * timeinfo;
	timeinfo = localtime(&rawtime);
	
	std::string buffer;
	buffer.resize(format.size()+1);
	int len = strftime(&buffer[0], buffer.size(), format.c_str(),
			   timeinfo);
	while (len == 0) {
		buffer.resize(buffer.size()*2);
		len = strftime(&buffer[0], buffer.size(), format.c_str(), 
				timeinfo);
	}
	buffer.resize(len);
	return buffer;
}

std::string removeExtension(const std::string& s)
{
	std::size_t lastindx = s.find_last_of(".");
	if (lastindx == std::string::npos) return s;
	else return s.substr(0, lastindx);
}

std::string getExtension(const std::string& s)
{
	std::size_t lastindx = s.find_last_of(".");
	if (lastindx == std::string::npos) return default_ext;
	else return s.substr(lastindx, s.length());
}

std::string getFilename(const std::string& filename)
{
	// store extension
	std::ostringstream rstr;

	rstr << removeExtension(filename);
	rstr << "-";
	rstr << TimeString("%F-%R");
	rstr << getExtension(filename);
	
	DEBUG2(rstr.str());
	DEBUG2(rstr.str().length());
	
	return rstr.str();
}

std::string getFilePath(const path& m_path, const std::string& s)
{
	DEBUG("getFilepath\n");

	path dirpath = current_path() / m_path;
	
	if (!exists(dirpath))
		create_directory(dirpath);
	else
		DEBUG("Directory already exists!\n");
	
	DEBUG2((dirpath / s).string());
	return (dirpath / s).string();
}

} // end detail namespace

class File {
public:
	File() = delete;
	
	File(const std::string& filename, path m_dirpath, 
	     std::ios_base::openmode mode)
	: file(detail::getFilePath(m_dirpath, 
				    detail::getFilename(filename)),
		mode ),
	m_name(filename)
	{}
		
	File(const std::string& filename)
	: File(filename, detail::default_dir_name, std::ios_base::out)
	{}
	
	File(const std::string& filename, std::ios_base::openmode mode)
	: File(filename, detail::default_dir_name, mode)
	{}
	
	~File() { file.close();	}

	std::ofstream& getStream() 
	{
		return file;
	}
	
	const std::string name(void) const { return m_name; }
	
private:
	std::ofstream file;
	const std::string m_name;
};

template<typename T>
File& operator<<(File& filestream, T const& val)
{
	filestream.getStream() << val;
	return filestream;
}
	
File& operator<<(File& filestream, std::ostream& (*fn)(std::ostream&))
{
	filestream.getStream() << fn;
	return filestream;
}

} // end FileHelper namespace

#endif