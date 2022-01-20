#pragma once

#include <exception>
#include <stdexcept>
#include <string>

class FileException : public std::runtime_error
{
public:
	FileException(const std::string& err)
		: std::runtime_error{ err.c_str() } { }
};