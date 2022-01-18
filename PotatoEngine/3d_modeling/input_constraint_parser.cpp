#include "input_constraint_parser.hpp"
#include "event.hpp"
#include "parse_exception.hpp"

#include "GLFW/glfw3.h"

#include <map>
#include <string>

InputConstraintParser::InputConstraint InputConstraintParser::parseInputConstraint(const std::string& inputConstraint)
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

	InputConstraint constraint{};
	constraint.inputSeq.push_back(Event(Event::Type::KEY_RELEASE, GLFW_KEY_A));
	return constraint;
}