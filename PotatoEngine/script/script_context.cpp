#include "script_context.hpp"
#include "parse_exception.hpp"

ScriptContext::ScriptContext(std::ostream* outStream)
	: outStream(outStream) { }

BaseType* ScriptContext::getVariable(const std::string& name)
{
	if (variables.count(name) != 0)
		return variables.at(name).get();
	else
		throw ParseException("Unknown identifier: " + name);
}

BaseType* ScriptContext::createVariable(const std::string& name, std::unique_ptr<BaseType>&& var)
{
	variables[name] = std::move(var);
	return variables.at(name).get();
}
