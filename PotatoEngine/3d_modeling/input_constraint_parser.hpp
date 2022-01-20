#pragma once

#include "event.hpp"

#include <string>
#include <vector>
#include <array>

struct InputConstraint
{
	std::vector<Event> inputSeq;
	std::string command;
};

class InputConstraintParser
{
public:
	static InputConstraint parseInputConstraint(const std::string& inputConstraint);

private:
	static std::array<std::string, 3> separateConstraint(const std::string& inputConstraint);

	static std::vector<Event> generateInputSeq(const std::string& keyPart);

	static std::string generateCommand(const std::string& requirmentPart, const std::string& commandPart);
};
