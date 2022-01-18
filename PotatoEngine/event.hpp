#pragma once

#include "glm\glm.hpp"

struct Event
{
	enum class Type
	{
		UNKNOWN,
		KEY_PRESS,
		KEY_REPEAT,
		KEY_RELEASE,
		MOUSE_PRESS,
		MOUSE_RELEASE,
		MOUSE_MOVE,
		FRAME_BUFFER_RESIZE,
		SCROLL,
		CHAR,
	};

	Event();
	Event(Type type);
	Event(Type type, int key, int mods = 0);
	Event(Type type, int button, const glm::dvec2& pos, int mods);
	Event(Type type, const glm::dvec2& pos);
	Event(Type type, const glm::ivec2& size);
	Event(Type type, double scroll);
	Event(Type type, unsigned character);

	Type type;

	union
	{
		int key;
		struct
		{
			int button;
			glm::dvec2 pos;
		};
		glm::ivec2 size;
		double scroll;
		unsigned character;
	};

	int mods;

	bool operator==(const Event& e) const;
};