#include "assignment.hpp"
#include "integer_type.hpp"
#include "void_type.hpp"
#include "parse_exception.hpp"
#include "string_type.hpp"

std::unique_ptr<BaseType> Assignment::assign(ScriptContext* context, const std::string& identifier, BaseType* b0)
{
	context->createVariable(identifier, b0->copy());

	return std::make_unique<VoidType>();
}

std::unique_ptr<BaseType> Assignment::addAssign(ScriptContext* context, const std::string& identifier, BaseType* b0)
{
	auto ptr = context->getVariable(identifier);
	if (ptr->dynamicType == BaseType::DynamicType::INTEGER && b0->dynamicType == BaseType::DynamicType::INTEGER)
		dynamic_cast<IntegerType*>(ptr)->value += dynamic_cast<IntegerType*>(b0)->value;
	else
		throw ParseException("No operator += defined for ...");

	return std::make_unique<VoidType>();
}

std::unique_ptr<BaseType> Assignment::subAssign(ScriptContext* context, const std::string& identifier, BaseType* b0)
{
	auto ptr = context->getVariable(identifier);
	if (ptr->dynamicType == BaseType::DynamicType::INTEGER && b0->dynamicType == BaseType::DynamicType::INTEGER)
		dynamic_cast<IntegerType*>(ptr)->value -= dynamic_cast<IntegerType*>(b0)->value;
	else
		throw ParseException("No operator -= defined for ...");

	return std::make_unique<VoidType>();
}

std::unique_ptr<BaseType> Assignment::multAssign(ScriptContext* context, const std::string& identifier, BaseType* b0)
{
	auto ptr = context->getVariable(identifier);
	if (ptr->dynamicType == BaseType::DynamicType::INTEGER && b0->dynamicType == BaseType::DynamicType::INTEGER)
		dynamic_cast<IntegerType*>(ptr)->value *= dynamic_cast<IntegerType*>(b0)->value;
	else
		throw ParseException("No operator *= defined for ...");

	return std::make_unique<VoidType>();
}

std::unique_ptr<BaseType> Assignment::divAssign(ScriptContext* context, const std::string& identifier, BaseType* b0)
{
	auto ptr = context->getVariable(identifier);
	if (ptr->dynamicType == BaseType::DynamicType::INTEGER && b0->dynamicType == BaseType::DynamicType::INTEGER)
		dynamic_cast<IntegerType*>(ptr)->value /= dynamic_cast<IntegerType*>(b0)->value;
	else
		throw ParseException("No operator /= defined for ...");

	return std::make_unique<VoidType>();
}