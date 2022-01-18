#include "tokenizer.hpp"
#include "parse_exception.hpp"

#include <iostream>
#include <array>
#include <cassert>

std::vector<std::string> Tokenizer::tokenize(const std::string& input, const std::set<char>& valid,
	const std::set<char>& split, const std::set<std::string>& splitTokens, const int maxSplitTokenLength)
{
	if (!Tokenizer::validate(input, valid))
		throw ParseException("Tokenizer invalid character in input string");

	auto splitInput = Tokenizer::splitIgnore(input, split);

	std::vector<std::string> tokenized;
	for (auto part : splitInput)
	{
		auto tokens = splitToken(part, splitTokens, maxSplitTokenLength);
		tokenized.insert(tokenized.end(), tokens.begin(), tokens.end());
	}

	return tokenized;
}

bool Tokenizer::validate(const std::string& input, const std::set<char>& valid)
{
	for (auto ch : input)
		if (valid.count(ch) == 0)
			return false;

	return true;
}

std::vector<std::string> Tokenizer::splitIgnore(const std::string& input, const std::set<char>& split)
{
	std::vector<std::string> splitInput;
	std::string part = "";
	for (auto ch : input)
	{
		if (split.count(ch) != 0)
		{
			if (!part.empty())
			{
				splitInput.push_back(part);
				part.clear();
			}
		}
		else
			part += ch;
	}

	if (!part.empty())
		splitInput.push_back(part);

	return splitInput;
}

std::vector<std::string> Tokenizer::splitToken(const std::string& input, const std::set<std::string>& splitTokens, const int maxSplitTokenLength)
{
	auto findFirstLongest = [&](const std::string& sub) -> std::pair<int, std::string>
	{
		for (int startIndex = 0; startIndex < sub.size(); startIndex++)
		{
			for (int len = maxSplitTokenLength; len > 0; len--)
			{
				auto subsub = sub.substr(startIndex, len);
				if (splitTokens.count(subsub) != 0)
					return std::make_pair(startIndex, subsub);
			}
		}

		return std::make_pair(-1, "");
	};

	std::vector<std::string> tokens;

	auto currentSubstr = input;
	while (!currentSubstr.empty())
	{
		auto [index, str] = findFirstLongest(currentSubstr);

		if (index == -1)
		{
			tokens.push_back(currentSubstr);
			currentSubstr.clear();
		}
		else
		{
			if (index != 0)
				tokens.push_back(currentSubstr.substr(0, index));

			tokens.push_back(str);

			currentSubstr = currentSubstr.substr(index + str.size());
		}
	}

	return tokens;
}