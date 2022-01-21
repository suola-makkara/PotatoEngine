#include "config.hpp"
#include "parse_exception.hpp"

#include <fstream>
#include <string>
#include <sstream>
#include <exception>
#include <iostream>

Config::Config()
{
	static const std::string defaultConfigFile = "config/config.txt";
	loadFromFile(defaultConfigFile);
}

void Config::loadFromFile(const std::string& file)
{
	std::fstream stream(file);
	if (!stream.is_open())
		std::cout << "Could not open file: " + file + "\n";

	std::vector<InputConstraint> newConstraints;
	std::string line;
	int lineNumber = 1;

	while (std::getline(stream, line))
	{
		if (!line.empty())
		{
			try
			{
				newConstraints.push_back(InputConstraintParser::parseInputConstraint(line));
			}
			catch (const ParseException& ex)
			{
				std::cout << "Error in parsing file " << file << " on line " << lineNumber << ": " << ex.what() << '\n';
			}
		}

		lineNumber++;
	}

	constraints = newConstraints;
}
