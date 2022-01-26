#pragma once

struct Storage
{
	enum class StorageType
	{
		NONE,
		VOID,
		INTEGER,
		STRING,
	};

	enum StorageCategory
	{
		NONE,
		READONLY = 1 << 0,
		//DYNAMIC  = 1 << 1,
	};

	using ValuePtr = void*;

	Storage(StorageType type, ValuePtr valuePtr = nullptr, size_t size = 1, StorageCategory category = NONE)
		: type(type), valuePtr(valuePtr), size(size), category(category) { }

	StorageType type;
	ValuePtr valuePtr;
	size_t size;
	StorageCategory category;
};
