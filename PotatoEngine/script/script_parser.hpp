#pragma once

#include "nonconstructible.hpp"

#include <string>

class ScriptParser : public Nonconstructible
{
public:
	static void parseSource(const std::string& source);
};
