#pragma once

#include <cstddef>
#include <stack>
#include <string>
#include <memory>

class ProgramStack
{
public:
	ProgramStack(size_t size) : memory(std::make_unique<std::byte[]>(size)) { }

	void push()
	{
		ptrs.push(ptr);
	}

	void pop()
	{
		ptr = ptrs.top();
		ptrs.pop();
	}
	
	template<typename T>
	T* allocate(size_t size = 1)
	{
		auto ret = new (memory.get() + ptr) T[size];
		ptr += size * sizeof(T);
		return ret;
	}

private:
	std::unique_ptr<std::byte[]> memory;
	size_t ptr{};
	std::stack<size_t> ptrs{};
};
