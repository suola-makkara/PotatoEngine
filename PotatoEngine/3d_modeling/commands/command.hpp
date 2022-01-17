#pragma once

#include "editor_context.hpp"

class Command
{
public:
	/*
	*	returns true in case state of the context was changed
	*   other wise returns false (EXCEPTION redo and revert commands)
	*/
	virtual bool execute(EditorContext& context) = 0;

	/*
	*	return true in case state of the context was changed
	*	other wise returns false (EXCEPTION redo and revert commands)
	* 
	*	after executing the state of context is same
	*	as before calling execute provided context is
	*	in immediate state resulting from execute with return value true
	*/
	virtual bool revert(EditorContext& context) = 0;
};