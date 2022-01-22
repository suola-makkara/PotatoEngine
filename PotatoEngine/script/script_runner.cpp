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
			//else
			//	throw ParseException("Unrecognized identifier: " + identifier->name);
		}
		//else
		
		evaluated.push_back(std::move(part));
	}
	
	return evaluated;
}

std::unique_ptr<BaseType> ScriptRunner::evaluateOperators(std::vector<std::unique_ptr<ScriptParser::ParseStruct>>&& statement)
{
	validateOperatorSyntax(statement);

	auto retMultDiv = evaluateMultDiv(std::move(statement));
	auto retAddSub = evalueteAddSub(std::move(retMultDiv));
	auto retAssign = evaluateAssignment(std::move(retAddSub));

	return getPtr(retAssign.back().get())->copy();
}

void ScriptRunner::validateOperatorSyntax(const std::vector<std::unique_ptr<ScriptParser::ParseStruct>>& statement)
{
	// ...
}

std::vector<std::unique_ptr<ScriptParser::ParseStruct>> ScriptRunner::evaluateMultDiv(std::vector<std::unique_ptr<ScriptParser::ParseStruct>>&& statement)
{
	std::vector<std::unique_ptr<ScriptParser::ParseStruct>> evaluated;
	for (int partIndex = 0; partIndex < statement.size(); partIndex++)
	{
		auto& part = statement[partIndex];

		if (part->type == PSType::OPERATOR)
		{
			auto opType = dynamic_cast<SP::Operator*>(part.get())->operatorType;

			if (opType == OPType::MULTIPLICATION || opType == OPType::DIVISION)
			{
				if (evaluated.size() > 0 && partIndex + 1 < statement.size() &&
					evaluated.back()->type == PSType::LITERAL && statement[partIndex + 1]->type == PSType::LITERAL)
				{
					auto left = getPtr(evaluated.back().get());
					auto right = getPtr(statement[partIndex + 1].get());

					std::unique_ptr<BaseType> ret;
					if (opType == OPType::MULTIPLICATION)
						ret = Arithmetic::multiplication(left, right);
					else
						ret = Arithmetic::division(left, right);

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

std::vector<std::unique_ptr<ScriptParser::ParseStruct>> ScriptRunner::evalueteAddSub(std::vector<std::unique_ptr<ScriptParser::ParseStruct>>&& statement)
{
	std::vector<std::unique_ptr<ScriptParser::ParseStruct>> evaluated;
	for (int partIndex = 0; partIndex < statement.size(); partIndex++)
	{
		auto& part = statement[partIndex];

		if (part->type == PSType::OPERATOR)
		{
			auto opType = dynamic_cast<SP::Operator*>(part.get())->operatorType;

			if (opType == OPType::ADDITION || opType == OPType::SUBTRACTION)
			{
				if (evaluated.size() > 0 && partIndex + 1 < statement.size())
				{
					auto left = getPtr(evaluated.back().get());
					auto right = getPtr(statement[partIndex + 1].get());

					std::unique_ptr<BaseType> ret;
					if (opType == OPType::ADDITION)
						ret = Arithmetic::addition(left, right);
					else
						ret = Arithmetic::subtraction(left, right);

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

std::vector<std::unique_ptr<ScriptParser::ParseStruct>> ScriptRunner::evaluateAssignment(std::vector<std::unique_ptr<ScriptParser::ParseStruct>>&& statement)
{
	std::vector<std::unique_ptr<ScriptParser::ParseStruct>> evaluated;
	for (int partIndex = 0; partIndex < statement.size(); partIndex++)
	{
		auto& part = statement[partIndex];

		if (part->type == PSType::OPERATOR)
		{
			auto opType = dynamic_cast<SP::Operator*>(part.get())->operatorType;

			if (opType == OPType::ASSIGNMENT || opType == OPType::ADD_ASSIGN || opType == OPType::SUB_ASSIGN || opType == OPType::MULT_ASSIGN || opType == OPType::DIV_ASSIGN)
			{
				if (evaluated.size() > 0 && partIndex + 1 < statement.size() && evaluated.back()->type == PSType::IDENTIFIER)
				{
					auto left = dynamic_cast<SP::Identifier*>(evaluated.back().get());
					auto right = getPtr(statement[partIndex + 1].get());

					std::unique_ptr<BaseType> ret;
					if (opType == OPType::ASSIGNMENT)
						ret = Assignment::assign(context.get(), left->name, right);
					else if (opType == OPType::ADD_ASSIGN)
						ret = Assignment::addAssign(context.get(), left->name, right);
					else if (opType == OPType::SUB_ASSIGN)
						ret = Assignment::subAssign(context.get(), left->name, right);
					else if (opType == OPType::MULT_ASSIGN)
						ret = Assignment::multAssign(context.get(), left->name, right);
					else
						ret = Assignment::divAssign(context.get(), left->name, right);

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
		return context->getVariable(dynamic_cast<SP::Identifier*>(part)->name);
	else
		throw ParseException("Expected literal or identifier");
}

std::vector<std::unique_ptr<ScriptParser::ParseStruct>> ScriptRunner::unpackExpression(std::unique_ptr<ScriptParser::ParseStruct>&& expression)
{
	return std::move(dynamic_cast<ScriptParser::Expression*>(expression.get())->parts);
}
