#include "arithmetic.hpp"
#include "integer_type.hpp"
#include "parse_exception.hpp"

std::unique_ptr<BaseType> Arithmetic::addition(ScriptContext*, BaseType* b0)
{
	std::unique_ptr<BaseType> ret;
	if (b0->dynamicType == BaseType::DynamicType::INTEGER)
		ret = std::make_unique<IntegerType>(dynamic_cast<IntegerType*>(b0)->value);
	else
		throw ParseException("No operator + defined for ...");

	return ret;
}

std::unique_ptr<BaseType> Arithmetic::subtraction(ScriptContext*, BaseType* b0)
{
	std::unique_ptr<BaseType> ret;
	if (b0->dynamicType == BaseType::DynamicType::INTEGER)
		ret = std::make_unique<IntegerType>(-dynamic_cast<IntegerType*>(b0)->value);
	else
		throw ParseException("No operator - defined for ...");

	return ret;
}

std::unique_ptr<BaseType> Arithmetic::addition(ScriptContext*, BaseType* b0, BaseType* b1)
{
	std::unique_ptr<BaseType> ret;
	if (b0->dynamicType == BaseType::DynamicType::INTEGER && b1->dynamicType == BaseType::DynamicType::INTEGER)
		ret = std::make_unique<IntegerType>(dynamic_cast<IntegerType*>(b0)->value + dynamic_cast<IntegerType*>(b1)->value);
	else
		throw ParseException("No operator + defined for ...");

	return ret;
}

std::unique_ptr<BaseType> Arithmetic::subtraction(ScriptContext*, BaseType* b0, BaseType* b1)
{
	std::unique_ptr<BaseType> ret;
	if (b0->dynamicType == BaseType::DynamicType::INTEGER && b1->dynamicType == BaseType::DynamicType::INTEGER)
		ret = std::make_unique<IntegerType>(dynamic_cast<IntegerType*>(b0)->value - dynamic_cast<IntegerType*>(b1)->value);
	else
		throw ParseException("No operator - defined for ...");

	return ret;
}

std::unique_ptr<BaseType> Arithmetic::multiplication(ScriptContext*, BaseType* b0, BaseType* b1)
{
	std::unique_ptr<BaseType> ret;
	if (b0->dynamicType == BaseType::DynamicType::INTEGER && b1->dynamicType == BaseType::DynamicType::INTEGER)
		ret = std::make_unique<IntegerType>(dynamic_cast<IntegerType*>(b0)->value * dynamic_cast<IntegerType*>(b1)->value);
	else
		throw ParseException("No operator * defined for ...");

	return ret;
}

std::unique_ptr<BaseType> Arithmetic::division(ScriptContext*, BaseType* b0, BaseType* b1)
{
	std::unique_ptr<BaseType> ret;
	if (b0->dynamicType == BaseType::DynamicType::INTEGER && b1->dynamicType == BaseType::DynamicType::INTEGER)
		ret = std::make_unique<IntegerType>(dynamic_cast<IntegerType*>(b0)->value / dynamic_cast<IntegerType*>(b1)->value);
	else
		throw ParseException("No operator / defined for ...");

	return ret;
}
