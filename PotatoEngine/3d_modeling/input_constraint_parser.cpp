#include "input_constraint_parser.hpp"
#include "event.hpp"
#include "parse_exception.hpp"
#include "tokenizer.hpp"

#include "GLFW/glfw3.h"

#include <map>
#include <string>
#include <cmath>

InputConstraintParser::InputConstraint InputConstraintParser::parseInputConstraint(const std::string& inputConstraint)
{
	auto [keyPart, requirmentPart, commandPart] = separateConstraint(inputConstraint);

	return InputConstraint{ generateInputSeq(keyPart), generateCommand(requirmentPart, commandPart) };
}

std::array<std::string, 3> InputConstraintParser::separateConstraint(const std::string& inputConstraint)
{
	static auto bracketSearch = [](const std::string& str, int off, const char left, const char right) -> std::pair<int, int>
	{
		int start = -1;
		int end = -1;
		int leftCount = 0;
		int rightCount = 0;
		for (int index = off; index < str.size(); index++)
		{
			auto chr = str[index];
			if (leftCount == 0 && chr == left)
			{
				start = index;
				leftCount++;
			}
			else if (leftCount != 0)
			{
				if (chr == left)
					leftCount++;
				else if (chr == right)
				{
					rightCount++;
					if (leftCount == rightCount)
					{
						end = index;
						break;
					}
				}
			}
		}

		return std::make_pair(start, end);
	};

	auto ex = ParseException("Error in separating constraint");

	std::array<std::string, 3> parts;

	// extract first part (...)
	auto nextStart = 0;
	auto [firstStart, firstEnd] = bracketSearch(inputConstraint, nextStart, '(', ')');
	if (firstStart == -1 || firstEnd == -1 || firstStart != nextStart)
		throw ex;
	auto firstPart = inputConstraint.substr(firstStart + 1, std::max(0, firstEnd - firstStart - 1));

	// extract second part [...] optional
	nextStart = firstEnd + 1;
	std::string secondPart = "";
	auto [secondStart, secondEnd] = bracketSearch(inputConstraint, nextStart, '[', ']');
	if (secondStart != -1 && secondEnd != -1 && secondStart == nextStart)
	{
		secondPart = inputConstraint.substr(secondStart + 1, std::max(0, secondEnd - secondStart - 1));

		nextStart = secondEnd + 1;
	}

	// extract third part {...}
	auto [thirdStart, thirdEnd] = bracketSearch(inputConstraint, nextStart, '{', '}');
	if (thirdStart == -1 || thirdEnd == -1 || thirdStart != nextStart)
		throw ex;
	auto thirdPart = inputConstraint.substr(thirdStart + 1, std::max(0, thirdEnd - thirdStart - 1));

	return std::array{ firstPart, secondPart, thirdPart };
}

std::vector<Event> InputConstraintParser::generateInputSeq(const std::string& keyPart)
{
	static const std::map<std::string, int> keyMap = []()
	{
		std::map<std::string, int> map;

		for (char num = GLFW_KEY_0; num <= GLFW_KEY_9; num++)
			map.insert(std::make_pair(std::string(1, num), num));
		for (char ch = GLFW_KEY_A; ch <= GLFW_KEY_Z; ch++)
			map.insert(std::make_pair(std::string(1, ch), ch));

		map["DELETE"] = GLFW_KEY_DELETE;
		map["ENTER"] = GLFW_KEY_ENTER;
		map["UP"] = GLFW_KEY_UP;
		map["DOWN"] = GLFW_KEY_DOWN;
		map["LEFT"] = GLFW_KEY_LEFT;
		map["RIGHT"] = GLFW_KEY_RIGHT;
		// add support for other keys here if needed
		//map[""] = GLFW_KEY_;

		return map;
	}();

	static const std::map<std::string, int> modMap = {
		{ "CTRL", GLFW_MOD_CONTROL },
		{ "SHIFT", GLFW_MOD_SHIFT },
		{ "ALT", GLFW_MOD_ALT }
	};

	static const std::set<char> validCharSet = [](const std::map<std::string, int>& keyMap, const std::map<std::string, int>& modMap)
	{
		std::set<char> s;
		for (const auto& pp : keyMap)
			for (auto ch : pp.first)
				s.insert(ch);

		for (const auto& pp : modMap)
			for (auto ch : pp.first)
				s.insert(ch);

		s.insert(' ');
		s.insert('+');

		return s;
	}(keyMap, modMap);

	static const std::set<char> splitCharSet = { ' ' };
	static const std::set<std::string> splitTokenSet = { "+" };

	auto tokens = Tokenizer::tokenize(keyPart, validCharSet, splitCharSet, splitTokenSet, 1);

	auto ex = ParseException("Error in generating input sequence");

	std::vector<Event> inputSeq;
	int currentMods = 0;
	bool prevAdd = false;
	for (const auto& token : tokens)
	{
		if (token == "+" && !prevAdd)
			prevAdd = true;
		else if (keyMap.count(token) != 0)
		{
			inputSeq.push_back(Event(Event::Type::KEY_RELEASE, keyMap.at(token), prevAdd ? currentMods : 0));
			prevAdd = false;
		}
		else if (modMap.count(token) != 0)
		{
			if (!prevAdd)
				currentMods = modMap.at(token);
			else
				currentMods |= modMap.at(token);
			prevAdd = false;
		}
		else
			throw ex;
	}

	return inputSeq;
}

std::string InputConstraintParser::generateCommand(const std::string& requirmentPart, const std::string& commandPart)
{
	if (commandPart.empty())
		return "";
	else if (requirmentPart.empty())
		return commandPart;
	else
		return "if(" + requirmentPart + "){" + commandPart + "}";
}
