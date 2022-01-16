#pragma once

#include "command.hpp"

class RevertCommand : public Command
{
public:
	bool execute(EditorContext& context) override;

	bool revert(EditorContext& context) override;
};