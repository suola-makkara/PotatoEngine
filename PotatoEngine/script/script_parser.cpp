#include "script_parser.hpp"
#include "tokenizer.hpp"
#include "parse_exception.hpp"
#include "string_type.hpp"
#include "integer_type.hpp"

#include <memory>

std::unique_ptr<ScriptParser::Expression> ScriptParser::parseSource(const std::string& source)
{
	static const std::set<char> validCharSet = []()
	{
		std::set<char> s;
		for (char ch = 'a'; ch <= 'z'; ch++)
			s.insert(ch);
		for (char ch = 'A'; ch <= 'Z'; ch++)
			s.insert(ch);
		for (char ch = '0'; ch <= '9'; ch++)
			s.insert(ch);

		s.insert({ '+', '-', '*', '/', ' ', '(', ')', '"', '\n', '\t', '\r'});

		return s;
	}();

	static const std::set<char> splitCharSet = { ' ', '\n', '\t', '\r' };
	static const std::set<std::string> splitTokenSet = { "+", "-", "*", "/", "(", ")", "\"" };

	auto tokens = Tokenizer::tokenize(source, validCharSet, splitCharSet, splitTokenSet, 1);

	return parseExpression(tokens);
}

std::unique_ptr<ScriptParser::Expression> ScriptParser::parseExpression(const std::vector<std::string>& tokens)
{
	static auto getUntil = [](const std::vector<std::string>& tokens, int index, const std::string& find) -> int {
		while (index < tokens.size())
		{
			if (tokens[index] == find)
				return index;
			index++;
		}

		return -1;
	};

	static auto getUntilClosed = [](const std::vector<std::string>& tokens, int index, const std::string& left, const std::string& right) -> int {
		int leftCount = 0;
		int rightCount = 0;
		while (index < tokens.size())
		{
			if (tokens[index] == left)
				leftCount++;
			else if (tokens[index] == right)
				rightCount++;

			if (leftCount == rightCount)
				return index;

			index++;
		}

		return -1;
	};

	static auto isValidIdentifier = [](const std::string& identifier) {
		bool first = true;
		for (auto ch : identifier)
		{
			if (!(('a' <= ch && ch <= 'z') || ('A' <= ch && ch <= 'Z') || ch == '_' || ('0' <= ch && ch <= '9' && !first)))
				return false;

			first = false;
		}
		return true;
	};

	static auto isValidInteger = [](const std::string& integer) {
		bool first = false;
		for (auto ch : integer)
		{
			if (!('0' <= ch && ch <= '9') || (ch == '0' && first))
				return false;

			first = false;
		}
		return true;
	};

	std::unique_ptr<Expression> expression = std::make_unique<Expression>();

	for (int tokenIndex = 0; tokenIndex < tokens.size(); tokenIndex++)
	{
		const auto& nextToken = tokens[tokenIndex];
		if (nextToken == "(")
		{
			int closeIndex = getUntilClosed(tokens, tokenIndex, "(", ")");
			if (closeIndex == -1)
				throw ParseException("Missing ')'");

			tokenIndex++;
			std::vector<std::string> passTokens;
			for (; tokenIndex < closeIndex; tokenIndex++)
				passTokens.push_back(tokens[tokenIndex]);

			expression->parts.push_back(parseExpression(passTokens));

			tokenIndex = closeIndex;
		}
		else if (nextToken == "\"")
		{
			int nextQuoteIndex = getUntil(tokens, tokenIndex + 1, "\"");
			if (nextQuoteIndex == -1)
				throw ParseException("Missing \"");

			std::string parsedString = "";
			tokenIndex++;
			for (; tokenIndex < nextQuoteIndex; tokenIndex++)
				parsedString += tokens[tokenIndex];

			expression->parts.push_back(std::make_unique<Literal>(std::make_unique<StringType>(parsedString)));

			tokenIndex = nextQuoteIndex;
		}
		else if (nextToken == "*" || nextToken == "/" || nextToken == "+" || nextToken == "-")
		{
			Operator::OperatorType operatorType;
			if (nextToken == "*")
				operatorType = Operator::OperatorType::MULTIPLICATION;
			else if (nextToken == "/")
				operatorType = Operator::OperatorType::DIVISION;
			else if (nextToken == "+")
				operatorType = Operator::OperatorType::ADDITION;
			else
				operatorType = Operator::OperatorType::SUBTRACTION;

			expression->parts.push_back(std::make_unique<Operator>(operatorType));
		}
		else if (isValidIdentifier(nextToken))
		{
			expression->parts.push_back(std::make_unique<Identifier>(nextToken));
		}
		else if (isValidInteger(nextToken))
		{
			int parsedInteger = std::stoi(nextToken);
			expression->parts.push_back(std::make_unique<Literal>(std::make_unique<IntegerType>(parsedInteger)));
		}
		else
			throw ParseException("Unexpected: " + nextToken);
	}

	return expression;
}
