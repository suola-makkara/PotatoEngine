#pragma once

#include "base_type.hpp"

#include <string>

class UninitializedType : public BaseType
{
public:
	UninitializedType(const std::string& identifier) : BaseType(DynamicType::UNINITIALIZED), identifier(identifier) { }

	std::string identifier;

	std::unique_ptr<BaseType> copy() const override { return std::make_unique<UninitializedType>(identifier); }
};
