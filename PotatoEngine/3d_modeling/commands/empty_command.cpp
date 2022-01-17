#include "empty_command.hpp"

#include <iostream>

bool EmptyCommand::execute(EditorContext& context)
{
	std::cout << "Unknown command\n";
	return false;
}

bool EmptyCommand::revert(EditorContext& context) { return false; }