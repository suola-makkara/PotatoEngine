#pragma once

#include "object.hpp"
#include "camera.hpp"

#include <memory>
#include <stack>

class Command;

struct EditorContext
{
	std::unique_ptr<Object> scene;

	enum class EditMode
	{
		OBJECT,
		VERTEX,
	};

	EditMode editMode = EditMode::OBJECT;

	// state varriables
	Object* selectedObject;

private:
	std::stack<std::unique_ptr<Command>> revertStack;
	std::stack<std::unique_ptr<Command>> redoStack;

	friend class RedoCommand;
	friend class RevertCommand;
	friend class Editor;
};