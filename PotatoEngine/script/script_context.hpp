#pragma once

#include "noncopyable.hpp"

#include <ostream>

class ScriptContext : public Noncopyable
{
public:
	ScriptContext(std::ostream* outStream);

	std::ostream* outStream;
};