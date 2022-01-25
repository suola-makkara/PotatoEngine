#include "base_type.hpp"

#include <string>
#include <map>

const std::string& BaseType::getTypeName() const
{
	static std::map<DynamicType, std::string> typeNames = {
		{DynamicType::INTEGER, "integer"}, {DynamicType::STRING, "string"},
		{DynamicType::VOID, "void"}, {DynamicType::FUNCTION, "function"},
		{DynamicType::UNINITIALIZED, "uninitialized"}
	};

	return typeNames[dynamicType];
}