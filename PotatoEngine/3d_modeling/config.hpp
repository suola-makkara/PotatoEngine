#pragma once

#include "input_constraint_parser.hpp"
#include "noncopyable.hpp"

#include <string>
#include <vector>

class Config : public Noncopyable
{
public:
	Config();

	void loadFromFile(const std::string& file);

private:
	std::vector<InputConstraint> constraints;
};
