#include "print.hpp"
#include "integer_type.hpp"
#include "string_type.hpp"
#include "script_context.hpp"

void Print::print(ScriptContext* context, BaseType* baseType)
{
	using DType = BaseType::DynamicType;

	switch (baseType->dynamicType)
	{
	case DType::INTEGER:
		(*context->outStream) << dynamic_cast<IntegerType*>(baseType)->value;
		break;
	case DType::STRING:
		(*context->outStream) << dynamic_cast<StringType*>(baseType)->value;
		break;
	}
}
