#pragma once

#include <string>
#include <vector>
#include <set>

class Tokenizer
{
public:
	/*
	* [input] string to tokenize
	* if character in [input] is not in [valid] throws ParseException
	* [split] characters on which [input] is split to tokens excluded from tokenization
	* [splitTokens] same as above except split on strings and append these to tokenization
	*/
	static std::vector<std::string> tokenize(const std::string& input, const std::set<char>& valid,
		const std::set<char>& split, const std::set<std::string>& splitTokens, const int maxSplitTokenLength);

private:
	static bool validate(const std::string& input, const std::set<char>& valid);

	static std::vector<std::string> splitIgnore(const std::string& input, const std::set<char>& split);

	static std::vector<std::string> splitToken(const std::string& input, const std::set<std::string>& splitTokens, const int maxSplitTokenLength);
};