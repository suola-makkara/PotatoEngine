#pragma once

#include "command.hpp"

class RedoCommand : public Command
{
public:
	bool execute(EditorContext& context) override;

	bool revert(EditorContext& context) override;
};