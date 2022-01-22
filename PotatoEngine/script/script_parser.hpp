#pragma once

#include "nonconstructible.hpp"
#include "base_type.hpp"
#include "noncopyable.hpp"

#include <string>
#include <vector>
#include <memory>

class ScriptParser : public Nonconstructible
{
public:
	class ParseStruct : Noncopyable
	{
	public:
		enum class Type
		{
			EXPRESSION,
			OPERATOR,
			IDENTIFIER,
			LITERAL
		};

		ParseStruct(Type type) : type(type) { }
		virtual ~ParseStruct() = default;

		const Type type;
	};

	class Expression : public ParseStruct
	{
	public:
		Expression() : ParseStruct(Type::EXPRESSION) { }

		std::vector<std::unique_ptr<ParseStruct>> parts;
	};

	class Literal : public ParseStruct
	{
	public:
		Literal(std::unique_ptr<BaseType>&& value) : ParseStruct(Type::LITERAL), value(std::move(value)) { }

		std::unique_ptr<BaseType> value;
	};

	class Operator : public ParseStruct
	{
	public:
		enum class OperatorType
		{
			ADDITION,
			SUBTRACTION,
			MULTIPLICATION,
			DIVISION
		};

		Operator(OperatorType operatorType) : ParseStruct(Type::OPERATOR), operatorType(operatorType) { }

		OperatorType operatorType;
	};

	class Identifier : public ParseStruct
	{
	public:
		Identifier(const std::string& name) : ParseStruct(Type::IDENTIFIER), name(name) { }

		std::string name;
	};

	static std::unique_ptr<ScriptParser::Expression> parseSource(const std::string& source);

private:
	static std::unique_ptr<ScriptParser::Expression> parseExpression(const std::vector<std::string>& tokens);
};
