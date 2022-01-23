#include "script_runner.hpp"
#include "script_parser.hpp"
#include "integer_type.hpp"
#include "string_type.hpp"
#include "void_type.hpp"
#include "parse_exception.hpp"
#include "print.hpp"
#include "arithmetic.hpp"
#include "assignment.hpp"
#include "println.hpp"
#include "uninitialized_type.hpp"

#include <list>

ScriptRunner::ScriptRunner(std::ostream* outStream)
{
	context = std::make_unique<ScriptContext>(outStream);
}

void ScriptRunner::run(const std::string& source)
{
	std::unique_ptr<ScriptParser::ParseStruct> expression = ScriptParser::parseSource(source);
	evaluateExpression(unpackExpression(std::move(expression)));
}

std::unique_ptr<BaseType> ScriptRunner::evaluateExpression(std::vector<std::unique_ptr<ScriptParser::ParseStruct>>&& expression)
{
	// split to expression to statements
	PSType prevType = PSType::OPERATOR;
	std::vector<std::vector<std::unique_ptr<SP::ParseStruct>>> statements;
	std::vector<std::unique_ptr<SP::ParseStruct>> statement;
	for (auto& part : expression)
	{
		const auto partType = part->type;
		if (partType != PSType::OPERATOR && prevType != PSType::OPERATOR)
		{
			if (prevType != PSType::IDENTIFIER || partType != PSType::EXPRESSION)
			{
				statements.push_back(std::move(statement));
				statement = std::vector<std::unique_ptr<SP::ParseStruct>>();
			}
		}

		statement.push_back(std::move(part));
		prevType = partType;
	}
	if (!statement.empty())
		statements.push_back(std::move(statement));

	// evaluate statements
	std::unique_ptr<BaseType> ret = std::make_unique<VoidType>();
	for (auto& statement : statements)
		ret = evaluateStatement(std::move(statement));

	// return return value of last statement
	return ret;
}

std::unique_ptr<BaseType> ScriptRunner::evaluateStatement(std::vector<std::unique_ptr<ScriptParser::ParseStruct>>&& statement)
{
	auto subexpRet = evaluateSubexpressions(std::move(statement));
	auto funcRet = evaluateFunctions(std::move(subexpRet));
	auto opRet = evaluateOperators(std::move(funcRet));

	return std::move(opRet);
}

std::vector<std::unique_ptr<ScriptParser::ParseStruct>> ScriptRunner::evaluateSubexpressions(std::vector<std::unique_ptr<ScriptParser::ParseStruct>>&& statement)
{
	std::vector<std::unique_ptr<SP::ParseStruct>> evaluated;
	for (auto& part : statement)
	{
		if (part->type == PSType::EXPRESSION)
			evaluated.push_back(std::make_unique<SP::Literal>(evaluateExpression(unpackExpression(std::move(part)))));
		else
			evaluated.push_back(std::move(part));
	}

	return evaluated;
}

std::vector<std::unique_ptr<ScriptParser::ParseStruct>> ScriptRunner::evaluateFunctions(std::vector<std::unique_ptr<ScriptParser::ParseStruct>>&& statement)
{
	std::vector<std::unique_ptr<SP::ParseStruct>> evaluated;
	for (int partIndex = 0; partIndex < statement.size(); partIndex++)
	{
		auto& part = statement[partIndex];

		if (part->type == PSType::IDENTIFIER)
		{
			SP::Identifier* identifier = dynamic_cast<SP::Identifier*>(part.get());
			if (identifier->name == "print" || identifier->name == "println")
			{
				if (partIndex + 1 >= statement.size())
					throw ParseException("Expected: (");
				auto& nextPart = statement[partIndex + 1];
				if (nextPart->type == PSType::LITERAL)
				{
					if (identifier->name == "print")
						Print::print(context.get(), dynamic_cast<SP::Literal*>(nextPart.get())->value.get());
					else
						Println::println(context.get(), dynamic_cast<SP::Literal*>(nextPart.get())->value.get());

					evaluated.push_back(std::make_unique<SP::Literal>(std::make_unique<VoidType>()));
					partIndex++;
				}
				else
					throw ParseException("Expected literal");

				continue;
			}
		}
		
		evaluated.push_back(std::move(part));
	}
	
	return evaluated;
}

std::unique_ptr<BaseType> ScriptRunner::evaluateOperators(std::vector<std::unique_ptr<ScriptParser::ParseStruct>>&& statement)
{
	validateOperatorSyntax(statement);

	auto retUnAddSub = evaluateUnaryAddSub(std::move(statement));
	auto retMultDiv = evaluateMultDiv(std::move(retUnAddSub));
	auto retAddSub = evalueteAddSub(std::move(retMultDiv));
	auto retAssign = evaluateAssignment(std::move(retAddSub));

	return getPtr(retAssign.back().get())->copy();
}

void ScriptRunner::validateOperatorSyntax(const std::vector<std::unique_ptr<ScriptParser::ParseStruct>>& statement)
{
	// TODO
}

std::vector<std::unique_ptr<ScriptParser::ParseStruct>> ScriptRunner::evaluateUnaryAddSub(std::vector<std::unique_ptr<SP::ParseStruct>>&& statement)
{
	static std::map<OPType, UnaryFunc> addSubOps = []() {
		std::map<OPType, UnaryFunc> m;
		m.insert(std::make_pair<OPType, UnaryFunc>(OPType::ADDITION, &Arithmetic::addition));
		m.insert(std::make_pair<OPType, UnaryFunc>(OPType::SUBTRACTION, &Arithmetic::subtraction));
		return m;
	}();

	return evaluateUnaryOperator(std::move(statement), addSubOps);
}

std::vector<std::unique_ptr<ScriptParser::ParseStruct>> ScriptRunner::evaluateMultDiv(std::vector<std::unique_ptr<ScriptParser::ParseStruct>>&& statement)
{
	static std::map<OPType, BinaryFunc> multDivOps = []() {
		std::map<OPType, BinaryFunc> m;
		m.insert(std::make_pair<OPType, BinaryFunc>(OPType::MULTIPLICATION, &Arithmetic::multiplication));
		m.insert(std::make_pair<OPType, BinaryFunc>(OPType::DIVISION, &Arithmetic::division));
		return m;
	}();

	return evaluateBinaryOperator(std::move(statement), multDivOps);
}

std::vector<std::unique_ptr<ScriptParser::ParseStruct>> ScriptRunner::evalueteAddSub(std::vector<std::unique_ptr<ScriptParser::ParseStruct>>&& statement)
{
	static std::map<OPType, BinaryFunc> addSubOps = []() {
		std::map<OPType, BinaryFunc> m;
		m.insert(std::make_pair<OPType, BinaryFunc>(OPType::ADDITION, &Arithmetic::addition));
		m.insert(std::make_pair<OPType, BinaryFunc>(OPType::SUBTRACTION, &Arithmetic::subtraction));
		return m;
	}();

	return evaluateBinaryOperator(std::move(statement), addSubOps);
}

std::vector<std::unique_ptr<ScriptParser::ParseStruct>> ScriptRunner::evaluateAssignment(std::vector<std::unique_ptr<ScriptParser::ParseStruct>>&& statement)
{
	static std::map<OPType, BinaryFunc> assignOps = []() {
		std::map<OPType, BinaryFunc> m;
		m.insert(std::make_pair<OPType, BinaryFunc>(OPType::ASSIGNMENT, &Assignment::assign));
		m.insert(std::make_pair<OPType, BinaryFunc>(OPType::ADD_ASSIGN, &Assignment::addAssign));
		m.insert(std::make_pair<OPType, BinaryFunc>(OPType::SUB_ASSIGN, &Assignment::subAssign));
		m.insert(std::make_pair<OPType, BinaryFunc>(OPType::MULT_ASSIGN,&Assignment::multAssign));
		m.insert(std::make_pair<OPType, BinaryFunc>(OPType::DIV_ASSIGN, &Assignment::divAssign));
		return m;
	}();

	return evaluateBinaryOperator(std::move(statement), assignOps);
}

std::vector<std::unique_ptr<ScriptParser::ParseStruct>> ScriptRunner::evaluateUnaryOperator(std::vector<std::unique_ptr<SP::ParseStruct>>&& statement, std::map<OPType, UnaryFunc>& opMap)
{
	std::vector<std::unique_ptr<ScriptParser::ParseStruct>> evaluated;
	for (int partIndex = 0; partIndex < statement.size(); partIndex++)
	{
		auto& part = statement[partIndex];

		if (part->type == PSType::OPERATOR)
		{
			auto opType = dynamic_cast<SP::Operator*>(part.get())->operatorType;

			if (opMap.count(opType) != 0 && (evaluated.size() == 0 || evaluated.back()->type == PSType::OPERATOR))
			{
				if (partIndex + 1 < statement.size())
				{
					auto right = getPtr(statement[partIndex + 1].get());

					auto ret = opMap.at(opType)(context.get(), right);

					evaluated.push_back(std::make_unique<SP::Literal>(std::move(ret)));
					partIndex++;
				}
				else
					throw ParseException("Expected literal");

				continue;
			}
		}

		evaluated.push_back(std::move(part));
	}

	return evaluated;
}

std::vector<std::unique_ptr<ScriptParser::ParseStruct>> ScriptRunner::evaluateBinaryOperator(std::vector<std::unique_ptr<SP::ParseStruct>>&& statement, std::map<OPType, BinaryFunc>& opMap)
{
	std::vector<std::unique_ptr<ScriptParser::ParseStruct>> evaluated;
	for (int partIndex = 0; partIndex < statement.size(); partIndex++)
	{
		auto& part = statement[partIndex];

		if (part->type == PSType::OPERATOR)
		{
			auto opType = dynamic_cast<SP::Operator*>(part.get())->operatorType;

			if (opMap.count(opType) != 0)
			{
				if (evaluated.size() > 0 && partIndex + 1 < statement.size())
				{
					auto left = getPtr(evaluated.back().get());
					auto right = getPtr(statement[partIndex + 1].get());

					auto ret = opMap.at(opType)(context.get(), left, right);

					evaluated.pop_back();
					evaluated.push_back(std::make_unique<SP::Literal>(std::move(ret)));
					partIndex++;
				}
				else
					throw ParseException("Expected literal");

				continue;
			}
		}

		evaluated.push_back(std::move(part));
	}

	return evaluated;
}

BaseType* ScriptRunner::getPtr(ScriptParser::ParseStruct* part)
{
	if (part->type == PSType::LITERAL)
		return dynamic_cast<SP::Literal*>(part)->value.get();
	else if (part->type == PSType::IDENTIFIER)
	{
		auto name = dynamic_cast<SP::Identifier*>(part)->name;
		if (context->variables.count(name) == 0)
			return context->createVariable(name, std::make_unique<UninitializedType>(name));
		else
			return context->getVariable(name);
	}
	else
		throw ParseException("Expected literal or identifier");
}

std::vector<std::unique_ptr<ScriptParser::ParseStruct>> ScriptRunner::unpackExpression(std::unique_ptr<ScriptParser::ParseStruct>&& expression)
{
	return std::move(dynamic_cast<ScriptParser::Expression*>(expression.get())->parts);
}
