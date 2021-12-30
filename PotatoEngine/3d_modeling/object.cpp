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

std::vector<glm::vec3> Object::selectVertices(const glm::vec2& start, const glm::vec2& size, const glm::mat4& projView)
{
	std::vector<glm::vec3> verts;
	for (auto child : children)
	{
		auto temp = child->selectVertices(start, size, projView);
		verts.insert(verts.end(), std::make_move_iterator(temp.begin()), std::make_move_iterator(temp.end()));
	}

	return verts;
}

void Object::add(Object* object)
{
	children.push_back(object);
}