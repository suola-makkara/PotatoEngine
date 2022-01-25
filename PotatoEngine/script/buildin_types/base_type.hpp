#pragma once
#include "noncopyable.hpp"

#include <memory>
#include <string>

class BaseType : public Noncopyable
{
public:
	enum class DynamicType
	{
		VOID,
		INTEGER,
		STRING,
		FUNCTION,
		UNINITIALIZED
	};

	BaseType(DynamicType dynamicType) : dynamicType(dynamicType) { }
	virtual ~BaseType() = default;

	virtual std::unique_ptr<BaseType> copy() const = 0;

	const std::string& getTypeName() const;

	const DynamicType dynamicType;
};
