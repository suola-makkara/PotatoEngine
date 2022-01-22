#pragma once

#include "base_type.hpp"

class IntegerType : public BaseType
{
public:
	IntegerType(int value) : BaseType(DynamicType::INTEGER), value(value) { }
	int value{};

	std::unique_ptr<BaseType> copy() const override { return std::make_unique<IntegerType>(value); }
};
