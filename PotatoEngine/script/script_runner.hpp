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
	using SP = ScriptParser;
	using PSType = SP::ParseStruct::Type;
	using OPType = SP::Operator::OperatorType;

	using UnaryFunc = std::unique_ptr<BaseType>(*)(ScriptContext*, BaseType*);
	using BinaryFunc = std::unique_ptr<BaseType>(*)(ScriptContext*, BaseType*, BaseType*);

	std::unique_ptr<ScriptContext> context;

	std::unique_ptr<BaseType> evaluateExpression(std::vector<std::unique_ptr<SP::ParseStruct>>&& expression);

	std::unique_ptr<BaseType> evaluateStatement(std::vector<std::unique_ptr<SP::ParseStruct>>&& statement);

	std::vector<std::unique_ptr<SP::ParseStruct>> evaluateSubexpressions(std::vector<std::unique_ptr<SP::ParseStruct>>&& statement);

	std::vector<std::unique_ptr<SP::ParseStruct>> evaluateFunctions(std::vector<std::unique_ptr<SP::ParseStruct>>&& statement);

	std::unique_ptr<BaseType> evaluateOperators(std::vector<std::unique_ptr<SP::ParseStruct>>&& statement);

	void validateOperatorSyntax(const std::vector<std::unique_ptr<SP::ParseStruct>>& statement);

	std::vector<std::unique_ptr<SP::ParseStruct>> evaluateUnaryAddSub(std::vector<std::unique_ptr<SP::ParseStruct>>&& statement);

	std::vector<std::unique_ptr<SP::ParseStruct>> evaluateMultDiv(std::vector<std::unique_ptr<SP::ParseStruct>>&& statement);

	std::vector<std::unique_ptr<SP::ParseStruct>> evalueteAddSub(std::vector<std::unique_ptr<SP::ParseStruct>>&& statement);

	std::vector<std::unique_ptr<SP::ParseStruct>> evaluateAssignment(std::vector<std::unique_ptr<SP::ParseStruct>>&& statement);


	std::vector<std::unique_ptr<SP::ParseStruct>> evaluateUnaryOperator(std::vector<std::unique_ptr<SP::ParseStruct>>&& statement, std::map<OPType, UnaryFunc>& opMap);

	std::vector<std::unique_ptr<SP::ParseStruct>> evaluateBinaryOperator(std::vector<std::unique_ptr<SP::ParseStruct>>&& statement, std::map<OPType, BinaryFunc>& opMap);


	BaseType* getPtr(SP::ParseStruct* part);

	std::vector<std::unique_ptr<SP::ParseStruct>> unpackExpression(std::unique_ptr<SP::ParseStruct>&& expression);
};
