#include "script_runner.hpp"
#include "script_parser.hpp"

ScriptRunner::ScriptRunner(std::ostream* outStream)
{
	context = std::make_unique<ScriptContext>(outStream);
}

void ScriptRunner::run(const std::string& source)
{
	ScriptParser::parseSource(source);

	// TODO
}
