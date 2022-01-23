#include "script_parser.hpp"
#include "tokenizer.hpp"
#include "parse_exception.hpp"
#include "string_type.hpp"
#include "integer_type.hpp"

#include <memory>
#include <map>
#include <cmath>

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

		s.insert({ '+', '-', '*', '/', ' ', '(', ')', '"', '=', '\n', '\t', '\r'});

		return s;
	}();

	static const std::set<char> splitCharSet = { ' ', '\n', '\t', '\r' };
	static const std::set<std::string> splitTokenSet = { "+", "-", "*", "/", "(", ")", "\"", "=", "+=", "-=", "*=", "/=" };

	std::string sourceCopy = source;
	auto strings = preprocessStrings(sourceCopy);
	auto tokens = Tokenizer::tokenize(sourceCopy, validCharSet, splitCharSet, splitTokenSet, 2);
	replaceStrings(tokens, std::move(strings));

	return parseExpression(tokens);
}

std::vector<std::string> ScriptParser::preprocessStrings(std::string& source)
{
	std::string replacedSource = "";
	std::vector<std::string> strings;

	size_t offset = 0;
	while (true)
	{
		auto start = source.find('"', offset);
		if (start != std::string::npos)
		{
			replacedSource += source.substr(offset, start - offset + 1);

			auto end = source.find('"', start + 1);
			if (end != std::string::npos)
			{
				strings.push_back(source.substr(start, end - start + 1));
				offset = end + 1;
			}
			else
				throw ParseException("Expected \"");
		}
		else
		{
			replacedSource += source.substr(offset);
			break;
		}
	}

	source = replacedSource;
	return strings;
}

void ScriptParser::replaceStrings(std::vector<std::string>& tokens, std::vector<std::string>&& strings)
{
	int stringIndex = 0;
	for (auto& token : tokens)
	{
		if (token == "\"")
		{
			token = std::move(strings[stringIndex]);
			stringIndex++;
		}
	}
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

	static const std::map<std::string, Operator::OperatorType> operators = {
		{ "+", Operator::OperatorType::ADDITION },
		{ "-", Operator::OperatorType::SUBTRACTION },
		{ "*", Operator::OperatorType::MULTIPLICATION },
		{ "/", Operator::OperatorType::DIVISION },
		{ "=", Operator::OperatorType::ASSIGNMENT },
		{ "+=", Operator::OperatorType::ADD_ASSIGN },
		{ "-=", Operator::OperatorType::SUB_ASSIGN },
		{ "*=", Operator::OperatorType::MULT_ASSIGN },
		{ "/=", Operator::OperatorType::DIV_ASSIGN }
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
		else if (nextToken[0] == '\"')
		{
			expression->parts.push_back(std::make_unique<Literal>(std::make_unique<StringType>(nextToken.substr(1, nextToken.size() - 2))));
		}
		else if (operators.count(nextToken) != 0)
		{
			Operator::OperatorType operatorType = operators.at(nextToken);
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
