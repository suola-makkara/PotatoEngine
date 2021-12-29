#include "object.hpp"

Object::~Object()
{
	for (auto child : children)
		delete child;
}

void Object::render(const Camera* camera) const
{
	for (auto child : children)
		child->render(camera);
}

void Object::add(Object* object)
{
	children.push_back(object);
}