#pragma once

#include "base_type.hpp"
#include "script_context.hpp"
#include "nonconstructible.hpp"

#include <memory>

class Arithmetic : public Nonconstructible
{
public:
	static std::unique_ptr<BaseType> addition(ScriptContext*, BaseType* b0);

	static std::unique_ptr<BaseType> subtraction(ScriptContext*, BaseType* b0);

	static std::unique_ptr<BaseType> addition(ScriptContext*, BaseType* b0, BaseType* b1);

	static std::unique_ptr<BaseType> subtraction(ScriptContext*, BaseType* b0, BaseType* b1);

	static std::unique_ptr<BaseType> multiplication(ScriptContext*, BaseType* b0, BaseType* b1);

	static std::unique_ptr<BaseType> division(ScriptContext*, BaseType* b0, BaseType* b1);
};
