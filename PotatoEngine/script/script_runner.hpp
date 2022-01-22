#pragma once

#include "script_context.hpp"
#include "base_type.hpp"
#include "script_parser.hpp"

#include <string>
#include <memory>
#include <ostream>

class ScriptRunner
{
public:
	ScriptRunner(std::ostream* outStream);

	void run(const std::string& source);

private:
	std::unique_ptr<ScriptContext> context;

	std::unique_ptr<BaseType> executeExpression(std::unique_ptr<ScriptParser::ParseStruct>&& expression);
};
