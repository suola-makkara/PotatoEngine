#pragma once

#include "base_type.hpp"

class IntegerType : public BaseType
{
public:
	IntegerType(int value) : BaseType(DynamicType::INTEGER), value(value) { }
	int value{};
};
