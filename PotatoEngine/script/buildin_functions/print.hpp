#pragma once

#include "base_type.hpp"
#include "script_context.hpp"
#include "nonconstructible.hpp"

#include <memory>

class Print : public Nonconstructible
{
public:
	static void print(ScriptContext* context, BaseType* baseType);
};