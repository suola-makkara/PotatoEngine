#pragma once

#include "event.hpp"

#include <string>
#include <vector>


class InputConstraintParser
{
public:
	struct InputConstraint
	{
		std::vector<Event> inputSeq;
		std::string command;
	};

	static InputConstraint parseInputConstraint(const std::string& inputConstraint);
};
