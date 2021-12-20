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
	};

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
		float scroll;
	};
};