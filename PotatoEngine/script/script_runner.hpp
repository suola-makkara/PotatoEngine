#pragma once

#include "script_context.hpp"

#include <string>
#include <memory>
#include <ostream>

class ScriptRunner
{
public:
	ScriptRunner(std::ostream* outStream);

	void run(const std::string& source);

private:
	std::unique_ptr<ScriptContext> context;
};
