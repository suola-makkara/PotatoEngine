#include "input_constraint_parser.hpp"
#include "event.hpp"

#include "GLFW/glfw3.h"

InputConstraintParser::InputConstraint InputConstraintParser::parseInputConstraint(const std::string& inputConstraint)
{
	InputConstraint constraint{};
	constraint.inputSeq.push_back(Event(Event::Type::KEY_RELEASE, GLFW_KEY_A));
	return constraint;
}