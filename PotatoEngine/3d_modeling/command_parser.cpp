#include "command_parser.hpp"
#include "test_command.hpp"
#include "empty_command.hpp"
#include "revert_command.hpp"
#include "redo_command.hpp"

#include <memory>

std::unique_ptr<Command> CommandParser::parseCommand(const std::string& commandString)
{
	if (commandString == "test")
		return std::make_unique<TestCommand>();
	if (commandString == "revert")
		return std::make_unique<RevertCommand>();
	if (commandString == "redo")
		return std::make_unique<RedoCommand>();

	return std::make_unique<EmptyCommand>();
}