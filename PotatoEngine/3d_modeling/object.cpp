#include "object.hpp"

#include "glm/gtx/transform.hpp"

Object::Object(Object&& obj) noexcept
{
	operator=(std::move(obj));
}

Object& Object::operator=(Object&& obj) noexcept
{
	transformCache = obj.transformCache;
	transformed = obj.transformed;
	position = obj.position;
	scale = obj.scale;
	basis = obj.basis;
	parent = obj.parent;
	children = std::move(obj.children);

	obj.parent = nullptr;

	return *this;
}

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
	object->parent = this;
	object->setTransformed();
	children.push_back(object);
}

void Object::remove(Object* object)
{
	for (auto it = children.begin(); it != children.end(); it++)
		if (*it == object)
		{
			delete *it;
			children.erase(it);
			break;
		}
}

const glm::mat4& Object::getTransform() const
{
	if (transformed)
	{
		if (parent != nullptr)
			transformCache = parent->getTransform() * glm::translate(position) * glm::mat4(basis) * glm::scale(scale);
		else
			transformCache = glm::translate(position) * glm::mat4(basis) * glm::scale(scale);
		transformed = false;
	}
	return transformCache;
}

void Object::setPosition(const glm::vec3& position)
{
	this->position = position;
	setTransformed();
}

void Object::setScale(const glm::vec3& scale)
{
	this->scale = scale;
	setTransformed();
}

void Object::rotate(const glm::vec3& axis, float angle)
{
	basis = glm::mat3(glm::rotate(angle, axis)) * basis;
	setTransformed();
}

void Object::setTransformed()
{
	for (auto child : children)
		child->setTransformed();
	transformed = true;
}