#pragma once

#include "noncopyable.hpp"
#include "base_type.hpp"

#include <ostream>
#include <vector>
#include <memory>
#include <string>
#include <map>

class ScriptContext : public Noncopyable
{
public:
	ScriptContext(std::ostream* outStream);

	std::ostream* outStream;

	BaseType* getVariable(const std::string& name);

	BaseType* createVariable(const std::string& name, std::unique_ptr<BaseType>&& var);

	std::map<std::string, std::unique_ptr<BaseType>> variables;
};