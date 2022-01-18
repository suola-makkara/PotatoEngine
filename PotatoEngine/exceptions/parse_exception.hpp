#pragma once

#include <exception>
#include <stdexcept>
#include <string>

class ParseException : public std::runtime_error
{
public:
	ParseException(const std::string& err)
		: std::runtime_error{ err.c_str() } { }
};