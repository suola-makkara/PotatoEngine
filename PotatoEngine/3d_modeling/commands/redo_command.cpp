#include "redo_command.hpp"

bool RedoCommand::execute(EditorContext& context)
{
	if (!context.redoStack.empty())
	{
		if (context.redoStack.top()->execute(context))
			context.revertStack.push(std::move(context.redoStack.top()));

		context.redoStack.pop();
	}

	return false;
}

bool RedoCommand::revert(EditorContext& context) { return false; }