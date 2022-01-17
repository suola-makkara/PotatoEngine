#pragma once

#include "command.hpp"

#include <string>
#include <memory>
#include <list>

class CommandParser
{
public:
	static std::list<std::unique_ptr<Command>> parseCommand(const std::string& commandString);
};