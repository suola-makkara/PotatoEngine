#pragma once

#include "base_type.hpp"
#include "script_context.hpp"
#include "nonconstructible.hpp"

#include <memory>

class Assignment : public Nonconstructible
{
public:
	static std::unique_ptr<BaseType> assign(ScriptContext* context, BaseType* left, BaseType* right);

	static std::unique_ptr<BaseType> addAssign(ScriptContext*, BaseType* left, BaseType* right);

	static std::unique_ptr<BaseType> subAssign(ScriptContext*, BaseType* left, BaseType* right);

	static std::unique_ptr<BaseType> multAssign(ScriptContext*, BaseType* left, BaseType* right);

	static std::unique_ptr<BaseType> divAssign(ScriptContext*, BaseType* left, BaseType* right);
};
