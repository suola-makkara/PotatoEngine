#include "command_parser.hpp"
#include "test_command.hpp"
#include "empty_command.hpp"
#include "revert_command.hpp"
#include "redo_command.hpp"

#include <memory>
#include <set>
#include <iostream>
#include <list>

/*
* edit context
* - access thrugh config file
* - no gurantees on state after undo / redo which affect only on scene context
* 
* scene context
* - full desctiption of the scene objects with object hierarchy
* - only mutable by commands with undo / redo
* - "global" immutable access
* 
* scene command
* - operate on the scene with out direct access to edit context
* 
* 
*				 user input
*					 |
*			|->	input config	command input
*			|		 |				  |
*			|	     --------|---------
*			|				 |
*			|		general script context
*			|			|				| <- mutable access
*			modify edit context		 execute scene edit command
* 
* user input
*	- key / mouse input
*	-> input config
*		- defines script that executes constrained on input and edit context
* 
* input config syntax
* (KEYS)[REQUIRMENTS]{EXPRESSION}
* 
* examples
* 
* keys
* - single key
*	(A)
* - multiple keys in sequence
*	(A B C)
* - key modifiers
*	(L_CTRL+A)
* - mouse move
*	(MOUSE_MOVE)
* - mouse button
*	(MOUSE_LEFT)
* 
* if multiple key sequences could match on input longes terminates and starts new
* ex: (A B C), (C D) with input A B C D selects (A B C) and leaves D in buffer (C, D) wont be matched
* 
* requirments (immutable access to editor state and other possible state value such as window size)
* -	check edit mode (automatic ==)
*	[EDIT_CONTEXT.EDIT_MODE OBJECT]
* - check if an object is selected (automatic != NULL)
*	[EDIT_CONTEXT.SELECTED_OBJECT]
* - compare window size (comparisons >, <, >=, <=, (implicit ==))
*	[WINDOW.SIZE_X > 100]
* 
* expression (in general any command executable on command line :expression)
* { EDIT_CONTEXT.SET_EDIT_MODE(EDIT_MODE_VERTEX); }
* 
* 
* scripting
*	-custom / python ?
*/

std::list<std::unique_ptr<Command>> CommandParser::parseCommand(const std::string& commandString)
{
	static const std::set<std::string> validTokens{ "+", "-", "*", "/", "(", ")", "{", "}", "[", "]", ">", "<", "=", ">=", "<=", "=="};

	std::string token = "";
	std::vector<std::string> tokens;

	for (int i = 0; i < commandString.size(); i++)
	{
		if (commandString[i] == ' ')
		{
			if (!token.empty())
			{
				tokens.push_back(token);
				token = "";
			}
		}
		else if ((validTokens.count(token) != 0 && validTokens.count(token + commandString[i]) == 0) ||
			(validTokens.count(token) == 0 && validTokens.count(std::string(1, commandString[i])) != 0))
		{
			if (!token.empty())
				tokens.push_back(token);

			token = commandString[i];
		}
		else
		{
			token += commandString[i];
		}
	}

	if (!token.empty())
		tokens.push_back(token);

	std::list<std::unique_ptr<Command>> commands;

	for (const auto& token : tokens)
	{
		if (token == "test")
			commands.push_back(std::make_unique<TestCommand>());
		else if (token == "revert")
			commands.push_back(std::make_unique<RevertCommand>());
		else if (token == "redo")
			commands.push_back(std::make_unique<RedoCommand>());
		else
			commands.push_back(std::make_unique<EmptyCommand>());
	}

	return commands;
}