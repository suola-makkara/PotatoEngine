#include "test_command.hpp"

#include <iostream>

bool TestCommand::execute(EditorContext& context)
{
	std::cout << "Executing\n";
	return true;
}

bool TestCommand::revert(EditorContext& context)
{
	std::cout << "Reverting\n";
	return true;
}