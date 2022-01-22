#pragma once

#include "base_type.hpp"
#include "script_context.hpp"
#include "nonconstructible.hpp"

#include <memory>

class Assignment : public Nonconstructible
{
public:
	static std::unique_ptr<BaseType> assign(ScriptContext* context, const std::string& identifier, BaseType* b0);

	static std::unique_ptr<BaseType> addAssign(ScriptContext* context, const std::string& identifier, BaseType* b0);

	static std::unique_ptr<BaseType> subAssign(ScriptContext* context, const std::string& identifier, BaseType* b0);

	static std::unique_ptr<BaseType> multAssign(ScriptContext* context, const std::string& identifier, BaseType* b0);

	static std::unique_ptr<BaseType> divAssign(ScriptContext* context, const std::string& identifier, BaseType* b0);
};
