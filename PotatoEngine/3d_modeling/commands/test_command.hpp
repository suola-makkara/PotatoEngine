#pragma once

#include "command.hpp"

class TestCommand : public Command
{
public:
	bool execute(EditorContext& context) override;

	bool revert(EditorContext& context) override;
};