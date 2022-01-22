#pragma once
#include "noncopyable.hpp"

class BaseType : public Noncopyable
{
public:
	enum class DynamicType
	{
		VOID,
		INTEGER,
		STRING,
		FUNCTION
	};

	BaseType(DynamicType dynamicType) : dynamicType(dynamicType) { }
	virtual ~BaseType() = default;

	const DynamicType dynamicType;
};
