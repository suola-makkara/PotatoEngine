#pragma once

#include "base_type.hpp"

class FunctionType : public BaseType
{
public:
	FunctionType() : BaseType(DynamicType::FUNCTION) { }
};
