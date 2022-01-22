#pragma once
#include "noncopyable.hpp"

#include <memory>

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

	virtual std::unique_ptr<BaseType> copy() const = 0;

	const DynamicType dynamicType;
};
