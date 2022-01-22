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
	using PSType = ScriptParser::ParseStruct::Type;
	using OPType = ScriptParser::Operator::OperatorType;
	using SP = ScriptParser;

	std::unique_ptr<ScriptContext> context;

	//std::unique_ptr<BaseType> executeExpression(std::unique_ptr<ScriptParser::ParseStruct>&& expression);

	std::unique_ptr<BaseType> evaluateExpression(std::vector<std::unique_ptr<ScriptParser::ParseStruct>>&& expression);

	std::unique_ptr<BaseType> evaluateStatement(std::vector<std::unique_ptr<ScriptParser::ParseStruct>>&& statement);

	std::vector<std::unique_ptr<ScriptParser::ParseStruct>> evaluateSubexpressions(std::vector<std::unique_ptr<ScriptParser::ParseStruct>>&& statement);

	std::vector<std::unique_ptr<ScriptParser::ParseStruct>> evaluateFunctions(std::vector<std::unique_ptr<ScriptParser::ParseStruct>>&& statement);

	std::unique_ptr<BaseType> evaluateOperators(std::vector<std::unique_ptr<ScriptParser::ParseStruct>>&& statement);

	void validateOperatorSyntax(const std::vector<std::unique_ptr<ScriptParser::ParseStruct>>& statement);

	std::vector<std::unique_ptr<ScriptParser::ParseStruct>> evaluateMultDiv(std::vector<std::unique_ptr<ScriptParser::ParseStruct>>&& statement);

	std::vector<std::unique_ptr<ScriptParser::ParseStruct>> evalueteAddSub(std::vector<std::unique_ptr<ScriptParser::ParseStruct>>&& statement);

	std::vector<std::unique_ptr<ScriptParser::ParseStruct>> evaluateAssignment(std::vector<std::unique_ptr<ScriptParser::ParseStruct>>&& statement);


	BaseType* getPtr(ScriptParser::ParseStruct* part);

	std::vector<std::unique_ptr<ScriptParser::ParseStruct>> unpackExpression(std::unique_ptr<ScriptParser::ParseStruct>&& expression);
};
