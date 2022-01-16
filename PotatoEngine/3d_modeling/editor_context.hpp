#pragma once

#include "object.hpp"
#include "camera.hpp"

#include <memory>
#include <stack>

class Command;

struct EditorContext
{
	std::unique_ptr<Object> scene;
	std::unique_ptr<Camera> camera;

private:
	std::stack<std::unique_ptr<Command>> revertStack;
	std::stack<std::unique_ptr<Command>> redoStack;

	friend class RedoCommand;
	friend class RevertCommand;
	friend class Editor;
};