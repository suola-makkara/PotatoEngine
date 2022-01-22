#pragma once

#include "base_type.hpp"

class VoidType : public BaseType
{
public:
	VoidType() : BaseType(DynamicType::VOID) { }
};
