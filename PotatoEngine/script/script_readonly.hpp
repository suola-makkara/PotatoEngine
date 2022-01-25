#pragma once

#include "script_stack.hpp"

class Readonly : private Stack
{
public:
	Readonly(size_t size) : Stack(size) { }

	const int* allocateInt(int value);

	const float* allocateFloat(float value);

	const char* allocateString(const char* value);
};
