#include "command_parser.hpp"
#include "test_command.hpp"
#include "empty_command.hpp"
#include "revert_command.hpp"
#include "redo_command.hpp"

#include <memory>
#include <set>
#include <iostream>
#include <list>

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