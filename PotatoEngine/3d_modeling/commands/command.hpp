#pragma once

#include "editor_context.hpp"

class Command
{
public:
	virtual bool execute(EditorContext& context) = 0;

	virtual bool revert(EditorContext& context) = 0;
};