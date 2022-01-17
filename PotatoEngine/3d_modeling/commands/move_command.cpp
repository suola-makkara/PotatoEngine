#include "move_command.hpp"

MoveCommand::MoveCommand(const glm::vec3& position)
	: position(position) { }

bool MoveCommand::execute(EditorContext& context)
{
	if (context.selectedObject)
	{
		originalPosition = context.selectedObject->getPosition();
		context.selectedObject->setPosition(position);
		return true;
	}

	return false;
}

bool MoveCommand::revert(EditorContext& context)
{
	context.selectedObject->setPosition(originalPosition);

	return true;
}