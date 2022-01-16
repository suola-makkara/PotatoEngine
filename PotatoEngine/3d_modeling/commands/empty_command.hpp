#pragma once

#include "command.hpp"

class EmptyCommand : public Command
{
public:
	bool execute(EditorContext& context) override;

	bool revert(EditorContext& context) override;
};