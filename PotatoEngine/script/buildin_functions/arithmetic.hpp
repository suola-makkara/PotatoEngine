#pragma once

#include "base_type.hpp"
#include "script_context.hpp"
#include "nonconstructible.hpp"

#include <memory>

class Arithmetic : public Nonconstructible
{
public:
	static std::unique_ptr<BaseType> addition(ScriptContext*, BaseType* right);

	static std::unique_ptr<BaseType> subtraction(ScriptContext*, BaseType* right);

	static std::unique_ptr<BaseType> addition(ScriptContext*, BaseType* left, BaseType* right);

	static std::unique_ptr<BaseType> subtraction(ScriptContext*, BaseType* left, BaseType* right);

	static std::unique_ptr<BaseType> multiplication(ScriptContext*, BaseType* left, BaseType* right);

	static std::unique_ptr<BaseType> division(ScriptContext*, BaseType* left, BaseType* right);
};
