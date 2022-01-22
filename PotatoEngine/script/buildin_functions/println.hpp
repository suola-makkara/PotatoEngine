#pragma once

#include "base_type.hpp"
#include "script_context.hpp"
#include "nonconstructible.hpp"

#include <memory>

class Println : public Nonconstructible
{
public:
	static void println(ScriptContext* context, BaseType* baseType);
};
