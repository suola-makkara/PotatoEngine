#pragma once

#include "command.hpp"

class MoveCommand : public Command
{
public:
	MoveCommand(const glm::vec3& position);

	bool execute(EditorContext& context) override;

	bool revert(EditorContext& context) override;

private:
	glm::vec3 originalPosition;
	glm::vec3 position;
};