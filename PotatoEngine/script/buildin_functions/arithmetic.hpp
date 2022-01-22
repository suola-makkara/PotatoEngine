#pragma once

#include "base_type.hpp"
#include "script_context.hpp"
#include "nonconstructible.hpp"

#include <memory>

class Arithmetic : public Nonconstructible
{
public:
	static std::unique_ptr<BaseType> addition(BaseType* b0, BaseType* b1);

	static std::unique_ptr<BaseType> subtraction(BaseType* b0, BaseType* b1);

	static std::unique_ptr<BaseType> multiplication(BaseType* b0, BaseType* b1);

	static std::unique_ptr<BaseType> division(BaseType* b0, BaseType* b1);
};
