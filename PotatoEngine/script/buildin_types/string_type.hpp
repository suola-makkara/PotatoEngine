#pragma once

#include "base_type.hpp"

#include <string>

class StringType : public BaseType
{
public:
	StringType(const std::string& value) : BaseType(DynamicType::STRING), value(value) { }
	std::string value{};

	std::unique_ptr<BaseType> copy() const override { return std::make_unique<StringType>(value); }
};
