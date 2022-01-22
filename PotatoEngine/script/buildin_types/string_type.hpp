#pragma once

#include "base_type.hpp"

#include <string>

class StringType : public BaseType
{
public:
	StringType(const std::string& value) : BaseType(DynamicType::STRING), value(value) { }
	std::string value{};
};
