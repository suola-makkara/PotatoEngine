#include "arithmetic.hpp"
#include "integer_type.hpp"
#include "parse_exception.hpp"

std::unique_ptr<BaseType> Arithmetic::addition(BaseType* b0, BaseType* b1)
{
	std::unique_ptr<BaseType> ret;
	if (b0->dynamicType == BaseType::DynamicType::INTEGER && b1->dynamicType == BaseType::DynamicType::INTEGER)
		ret = std::make_unique<IntegerType>(dynamic_cast<IntegerType*>(b0)->value + dynamic_cast<IntegerType*>(b1)->value);
	else
		throw ParseException("No operator + defined for ...");

	return ret;
}

std::unique_ptr<BaseType> Arithmetic::subtraction(BaseType* b0, BaseType* b1)
{
	std::unique_ptr<BaseType> ret;
	if (b0->dynamicType == BaseType::DynamicType::INTEGER && b1->dynamicType == BaseType::DynamicType::INTEGER)
		ret = std::make_unique<IntegerType>(dynamic_cast<IntegerType*>(b0)->value - dynamic_cast<IntegerType*>(b1)->value);
	else
		throw ParseException("No operator - defined for ...");

	return ret;
}

std::unique_ptr<BaseType> Arithmetic::multiplication(BaseType* b0, BaseType* b1)
{
	std::unique_ptr<BaseType> ret;
	if (b0->dynamicType == BaseType::DynamicType::INTEGER && b1->dynamicType == BaseType::DynamicType::INTEGER)
		ret = std::make_unique<IntegerType>(dynamic_cast<IntegerType*>(b0)->value * dynamic_cast<IntegerType*>(b1)->value);
	else
		throw ParseException("No operator * defined for ...");

	return ret;
}

std::unique_ptr<BaseType> Arithmetic::division(BaseType* b0, BaseType* b1)
{
	std::unique_ptr<BaseType> ret;
	if (b0->dynamicType == BaseType::DynamicType::INTEGER && b1->dynamicType == BaseType::DynamicType::INTEGER)
		ret = std::make_unique<IntegerType>(dynamic_cast<IntegerType*>(b0)->value / dynamic_cast<IntegerType*>(b1)->value);
	else
		throw ParseException("No operator / defined for ...");

	return ret;
}
