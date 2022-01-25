#include "assignment.hpp"
#include "integer_type.hpp"
#include "void_type.hpp"
#include "parse_exception.hpp"
#include "string_type.hpp"
#include "base_type.hpp"
#include "uninitialized_type.hpp"

std::unique_ptr<BaseType> Assignment::assign(ScriptContext* context, BaseType* left, BaseType* right)
{
	if (left->dynamicType == BaseType::DynamicType::UNINITIALIZED)
		context->createVariable(dynamic_cast<UninitializedType*>(left)->identifier, right->copy());
	else if (left->dynamicType == BaseType::DynamicType::INTEGER && right->dynamicType == BaseType::DynamicType::INTEGER)
		dynamic_cast<IntegerType*>(left)->value = dynamic_cast<IntegerType*>(right)->value;
	else if (left->dynamicType == BaseType::DynamicType::STRING && right->dynamicType == BaseType::DynamicType::STRING)
		dynamic_cast<StringType*>(left)->value = dynamic_cast<StringType*>(right)->value;
	else
		throw ParseException("No operator = defined for " + left->getTypeName() + " and " + right->getTypeName());

	return std::make_unique<VoidType>();
}

std::unique_ptr<BaseType> Assignment::addAssign(ScriptContext*, BaseType* left, BaseType* right)
{
	if (left->dynamicType == BaseType::DynamicType::INTEGER && right->dynamicType == BaseType::DynamicType::INTEGER)
		dynamic_cast<IntegerType*>(left)->value += dynamic_cast<IntegerType*>(right)->value;
	else
		throw ParseException("No operator += defined for " + left->getTypeName() + " and " + right->getTypeName());

	return std::make_unique<VoidType>();
}

std::unique_ptr<BaseType> Assignment::subAssign(ScriptContext*, BaseType* left, BaseType* right)
{
	if (left->dynamicType == BaseType::DynamicType::INTEGER && right->dynamicType == BaseType::DynamicType::INTEGER)
		dynamic_cast<IntegerType*>(left)->value -= dynamic_cast<IntegerType*>(right)->value;
	else
		throw ParseException("No operator -= defined for " + left->getTypeName() + " and " + right->getTypeName());

	return std::make_unique<VoidType>();
}

std::unique_ptr<BaseType> Assignment::multAssign(ScriptContext*, BaseType* left, BaseType* right)
{
	if (left->dynamicType == BaseType::DynamicType::INTEGER && right->dynamicType == BaseType::DynamicType::INTEGER)
		dynamic_cast<IntegerType*>(left)->value *= dynamic_cast<IntegerType*>(right)->value;
	else
		throw ParseException("No operator *= defined for " + left->getTypeName() + " and " + right->getTypeName());

	return std::make_unique<VoidType>();
}

std::unique_ptr<BaseType> Assignment::divAssign(ScriptContext*, BaseType* left, BaseType* right)
{
	if (left->dynamicType == BaseType::DynamicType::INTEGER && right->dynamicType == BaseType::DynamicType::INTEGER)
		dynamic_cast<IntegerType*>(left)->value /= dynamic_cast<IntegerType*>(right)->value;
	else
		throw ParseException("No operator /= defined for " + left->getTypeName() + " and " + right->getTypeName());

	return std::make_unique<VoidType>();
}