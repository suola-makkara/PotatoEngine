#include "println.hpp"
#include "print.hpp"
#include "script_context.hpp"

void Println::println(ScriptContext* context, BaseType* baseType)
{
	Print::print(context, baseType);
	(*context->outStream) << '\n';
}
