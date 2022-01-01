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

std::list<Object::VertexRef> Object::selectVertices(const glm::vec2& start, const glm::vec2& size, const glm::mat4& projView)
{
	std::list<VertexRef> verts;
	for (auto child : children)
	{
		auto temp = child->selectVertices(start, size, projView);
		verts.insert(verts.end(), std::make_move_iterator(temp.begin()), std::make_move_iterator(temp.end()));
	}

	return verts;
}

std::list<Object::ObjectRef> Object::selectObjects(const glm::vec2& screenCoord, const glm::mat4& projView, const glm::vec3& cameraPos)
{
	std::list<ObjectRef> objs;
	for (auto child : children)
	{
		auto temp = child->selectObjects(screenCoord, projView, cameraPos);
		objs.insert(objs.end(), std::make_move_iterator(temp.begin()), std::make_move_iterator(temp.end()));
	}

	return objs;
}

void Object::add(Object* object)
{
	children.push_back(object);
}