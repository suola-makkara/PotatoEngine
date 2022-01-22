#include "script_runner.hpp"
#include "script_parser.hpp"
#include "integer_type.hpp"
#include "string_type.hpp"
#include "void_type.hpp"
#include "parse_exception.hpp"
#include "print.hpp"
#include "arithmetic.hpp"

#include <list>

ScriptRunner::ScriptRunner(std::ostream* outStream)
{
	context = std::make_unique<ScriptContext>(outStream);
}

void ScriptRunner::run(const std::string& source)
{
	auto expression = ScriptParser::parseSource(source);
	executeExpression(std::move(expression));
}

std::unique_ptr<BaseType> ScriptRunner::executeExpression(std::unique_ptr<ScriptParser::ParseStruct>&& expression)
{
	using PSType = ScriptParser::ParseStruct::Type;
	using SP = ScriptParser;

	SP::Expression* castExpression = dynamic_cast<SP::Expression*>(expression.get());

	// First pass evaluate all expressions and replace with literals
	std::vector<std::unique_ptr<SP::ParseStruct>> firstPass;
	for (int partIndex = 0; partIndex < castExpression->parts.size(); partIndex++)
	{
		const auto& part = castExpression->parts[partIndex];
		switch (part->type)
		{
		case PSType::EXPRESSION:
		{
			firstPass.push_back(std::make_unique<SP::Literal>(executeExpression(std::move(castExpression->parts[partIndex]))));
			break;
		}
		default:
			firstPass.push_back(std::move(castExpression->parts[partIndex]));
			break;
		}
	}

	// Second pass evaluate all identifiers
	std::vector<std::unique_ptr<SP::ParseStruct>> secondPass;
	for (int partIndex = 0; partIndex < firstPass.size(); partIndex++)
	{
		const auto& part = firstPass[partIndex];

		switch (part->type)
		{
		case PSType::IDENTIFIER:
		{
			SP::Identifier* identifier = dynamic_cast<SP::Identifier*>(part.get());
			// For now support only single built in function
			if (identifier->name == "print")
			{
				if (partIndex >= firstPass.size())
					throw ParseException("Expected: (");
				const auto& nextPart = firstPass[partIndex + 1];
				if (nextPart->type == PSType::LITERAL)
				{
					Print::print(context.get(), dynamic_cast<SP::Literal*>(nextPart.get())->value.get());
					secondPass.push_back(std::make_unique<SP::Literal>(std::make_unique<VoidType>()));
					partIndex++;
				}
				else
					throw ParseException("Expected expression");
			}
			else
				throw ParseException("Unrecognized identifier: " + identifier->name);
			break;
		}
		default:
			secondPass.push_back(std::move(firstPass[partIndex]));
			break;
		}
	}

	// Third pass evaluate operators
	std::list<std::unique_ptr<BaseType>> args;
	std::list<SP::Operator*> ops;
	bool prevIsArg = false;
	for (int partIndex = 0; partIndex < secondPass.size(); partIndex++)
	{
		const auto& part = secondPass[partIndex];

		switch (part->type)
		{
		case PSType::LITERAL:
		{
			if (prevIsArg)
				throw ParseException("Unexpected literal");
			prevIsArg = true;

			args.push_back(std::move(dynamic_cast<SP::Literal*>(part.get())->value));
			break;
		}
		case PSType::OPERATOR:
		{
			if (!prevIsArg)
				throw ParseException("Unexpected operator");
			prevIsArg = false;

			ops.push_back(dynamic_cast<SP::Operator*>(part.get()));
			break;
		}
		}
	}

	if (ops.size() != args.size() - 1)
		throw ParseException("Expected literal");

	auto argIt = args.begin();
	auto opIt = ops.begin();
	while (opIt != ops.end())
	{
		auto opType = (*opIt)->operatorType;
		if (opType == SP::Operator::OperatorType::MULTIPLICATION || opType == SP::Operator::OperatorType::DIVISION)
		{

			auto arg0 = (argIt++)->get();
			auto arg1 = (argIt--)->get();

			if (opType == SP::Operator::OperatorType::MULTIPLICATION)
				(*argIt) = Arithmetic::multiplication(arg0, arg1);
			else
				(*argIt) = Arithmetic::division(arg0, arg1);

			argIt++;

			opIt = ops.erase(opIt);
			argIt = args.erase(argIt);
			argIt--;
		}
		else
		{
			argIt++;
			opIt++;
		}
	}

	argIt = args.begin();
	opIt = ops.begin();
	while (opIt != ops.end())
	{
		auto opType = (*opIt)->operatorType;

		auto arg0 = (argIt++)->get();
		auto arg1 = (argIt--)->get();

		if (opType == SP::Operator::OperatorType::ADDITION)
			(*argIt) = Arithmetic::addition(arg0, arg1);
		else
			(*argIt) = Arithmetic::subtraction(arg0, arg1);

		argIt++;

		opIt = ops.erase(opIt);
		argIt = args.erase(argIt);
		argIt--;
	}

	return args.size() > 0 ? std::move(args.front()) : std::make_unique<VoidType>();
}
