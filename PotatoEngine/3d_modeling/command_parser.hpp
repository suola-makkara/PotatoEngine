#pragma once

#include "command.hpp"

#include <string>
#include <memory>

class CommandParser
{
public:
	static std::unique_ptr<Command> parseCommand(const std::string& commandString);
};