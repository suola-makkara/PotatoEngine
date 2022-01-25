#pragma once

#include <cstddef>
#include <stack>
#include <string>
#include <memory>

class Stack
{
public:
	Stack(size_t size);

	void push();

	void pop();

	int* allocateInt(int value);

	float* allocateFloat(float value);

	char* allocateString(const char* value);

private:
	std::unique_ptr<std::byte[]> memory;
	size_t ptr{};
	std::stack<size_t> ptrs{};
};
