#include "arithmetic.hpp"
#include "integer_type.hpp"
#include "parse_exception.hpp"

std::unique_ptr<BaseType> Arithmetic::addition(ScriptContext*, BaseType* right)
{
	std::unique_ptr<BaseType> ret;
	if (right->dynamicType == BaseType::DynamicType::INTEGER)
		ret = std::make_unique<IntegerType>(dynamic_cast<IntegerType*>(right)->value);
	else
		throw ParseException("No operator + defined for " + right->getTypeName());

	return ret;
}

std::unique_ptr<BaseType> Arithmetic::subtraction(ScriptContext*, BaseType* right)
{
	std::unique_ptr<BaseType> ret;
	if (right->dynamicType == BaseType::DynamicType::INTEGER)
		ret = std::make_unique<IntegerType>(-dynamic_cast<IntegerType*>(right)->value);
	else
		throw ParseException("No operator - defined for " + right->getTypeName());

	return ret;
}

std::unique_ptr<BaseType> Arithmetic::addition(ScriptContext*, BaseType* left, BaseType* right)
{
	std::unique_ptr<BaseType> ret;
	if (left->dynamicType == BaseType::DynamicType::INTEGER && right->dynamicType == BaseType::DynamicType::INTEGER)
		ret = std::make_unique<IntegerType>(dynamic_cast<IntegerType*>(left)->value + dynamic_cast<IntegerType*>(right)->value);
	else
		throw ParseException("No operator + defined for " + left->getTypeName() + " and " + right->getTypeName());

	return ret;
}

std::unique_ptr<BaseType> Arithmetic::subtraction(ScriptContext*, BaseType* left, BaseType* right)
{
	std::unique_ptr<BaseType> ret;
	if (left->dynamicType == BaseType::DynamicType::INTEGER && right->dynamicType == BaseType::DynamicType::INTEGER)
		ret = std::make_unique<IntegerType>(dynamic_cast<IntegerType*>(left)->value - dynamic_cast<IntegerType*>(right)->value);
	else
		throw ParseException("No operator - defined for " + left->getTypeName() + " and " + right->getTypeName());

	return ret;
}

std::unique_ptr<BaseType> Arithmetic::multiplication(ScriptContext*, BaseType* left, BaseType* right)
{
	std::unique_ptr<BaseType> ret;
	if (left->dynamicType == BaseType::DynamicType::INTEGER && right->dynamicType == BaseType::DynamicType::INTEGER)
		ret = std::make_unique<IntegerType>(dynamic_cast<IntegerType*>(left)->value * dynamic_cast<IntegerType*>(right)->value);
	else
		throw ParseException("No operator * defined for " + left->getTypeName() + " and " + right->getTypeName());

	return ret;
}

std::unique_ptr<BaseType> Arithmetic::division(ScriptContext*, BaseType* left, BaseType* right)
{
	std::unique_ptr<BaseType> ret;
	if (left->dynamicType == BaseType::DynamicType::INTEGER && right->dynamicType == BaseType::DynamicType::INTEGER)
		ret = std::make_unique<IntegerType>(dynamic_cast<IntegerType*>(left)->value / dynamic_cast<IntegerType*>(right)->value);
	else
		throw ParseException("No operator / defined for " + left->getTypeName() + " and " + right->getTypeName());

	return ret;
}
