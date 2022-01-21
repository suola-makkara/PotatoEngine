#include "event.hpp"

Event::Event() : Event(Event::Type::UNKNOWN) { }

Event::Event(Type type) : type(type), pos(), button(), mods() { }

Event::Event(Type type, int key, int mods) : Event(type)
{ this->key = key, this->mods = mods; assert(type == Type::KEY_PRESS || type == Type::KEY_RELEASE || type == Type::KEY_REPEAT); };

Event::Event(Type type, int button, const glm::dvec2& pos, int mods) : Event(type)
{ this->button = button, this->pos = pos, this->mods = mods; assert(type == Type::MOUSE_PRESS || type == Type::MOUSE_RELEASE); }

Event::Event(Type type, const glm::dvec2& pos) : Event(type)
{ this->pos = pos; assert(type == Type::MOUSE_MOVE); }

Event::Event(Type type, const glm::ivec2& size) : Event(type)
{ this->size = size; assert(type == Type::FRAME_BUFFER_RESIZE); }

Event::Event(Type type, double scroll) : Event(type)
{ this->scroll = scroll; assert(type == Type::SCROLL); }

Event::Event(Type type, unsigned character) : Event(type)
{ this->character = character; assert(type == Type::CHAR); }

bool Event::operator==(const Event& e) const
{
	if (type != e.type)
		return false;

	return
		(type == Type::UNKNOWN ||
		((type == Type::KEY_PRESS || type == Type::KEY_RELEASE) && key == e.key && mods == e.mods) ||
		((type == Type::MOUSE_PRESS || type == Type::MOUSE_RELEASE) && button == e.button && pos == e.pos && mods == e.mods) ||
		(type == Type::MOUSE_MOVE && pos == e.pos) ||
		(type == Type::FRAME_BUFFER_RESIZE && size == e.size) ||
		(type == Type::SCROLL && scroll == e.scroll) ||
		(type == Type::CHAR && character == e.character));
}