#include "script_stack.hpp"

#include <cstddef>
#include <stack>
#include <cstring>

Stack::Stack(size_t size) : memory(std::make_unique<std::byte[]>(size)) { }

void Stack::push()
{
	ptrs.push(ptr);
}

void Stack::pop()
{
	ptr = ptrs.top();
	ptrs.pop();
}

int* Stack::allocateInt(int value)
{
	auto ret = new (memory.get() + ptr) int(value);
	ptr += sizeof(int);
	return ret;
}

float* Stack::allocateFloat(float value)
{
	auto ret = new (memory.get() + ptr) float(value);
	ptr += sizeof(float);
	return ret;
}

char* Stack::allocateString(const char* str)
{
	const auto len = strlen(str);
	auto ret = new (memory.get() + ptr) char[len + 1];
	strcpy(ret, str);
	ptr += sizeof(char) * (len + 1);
	return ret;
}
