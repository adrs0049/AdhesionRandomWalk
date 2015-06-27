/*
 * file.h
 * 
 * Some small helper functions used here and there
 * 
 * Andreas Buttenschoen 
 * 2014
 * 
 */

#ifndef FILE_H
#define FILE_H

#include <cstdio>
#include <string>
#include <stdexcept>

#define FILE_H_VERSION 0.1

class File {
	
public:
	File(const char* name, const char *mode) 
	: pFile{fopen(name, mode)}
	{
		if (pFile==nullptr) throw std::runtime_error{"File: Can't open file"};
	}
	
	File(const string& name, const char *mode)
	: File{name.c_str(), mode}
	{}
	
	explicit File(FILE * pp)
	: pFile{pp}
	{
		if(pFile==nullptr) throw std::runtime_error{"File: nullptr"};
	}
	
	File(const File& file_obj)
	: pFile{file_obj.pFile}
	{}
	
	File(const File&& file_obj)
	: pFile{file_obj.pFile}
	{
		file_obj.pFile = nullptr;
	}
	
	~File() { fclose(pFile); }
	
	operator FILE*() { return pFile; }
	
private:
	FILE * pFile;
};

#endif