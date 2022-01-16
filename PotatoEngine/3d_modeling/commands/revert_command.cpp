#include "revert_command.hpp"

bool RevertCommand::execute(EditorContext& context)
{
	if (!context.revertStack.empty())
	{
		if (context.revertStack.top()->revert(context))
			context.redoStack.push(std::move(context.revertStack.top()));

		context.revertStack.pop();
	}

	return false;
}

bool RevertCommand::revert(EditorContext& context) { return false; }