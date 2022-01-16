#include "empty_command.hpp"

bool EmptyCommand::execute(EditorContext& context) { return false; }

bool EmptyCommand::revert(EditorContext& context) { return false; }