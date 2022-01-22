#pragma once

#include "base_type.hpp"

class VoidType : public BaseType
{
public:
	VoidType() : BaseType(DynamicType::VOID) { }

	std::unique_ptr<BaseType> copy() const override { return std::make_unique<VoidType>(); }
};
