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
	tags = std::move(obj.tags);
	renderMode = obj.renderMode;

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

std::list<Object::ObjectRef> Object::selectObjects(const Ray& ray)
{
	std::list<ObjectRef> objs;
	for (auto child : children)
	{
		auto temp = child->selectObjects(ray);
		objs.insert(objs.end(), std::make_move_iterator(temp.begin()), std::make_move_iterator(temp.end()));
	}

	return objs;
}

void Object::setRenderMode(RenderMode mode)
{
	for (auto child : children)
		child->setRenderMode(mode);

	renderMode = mode;
}

Object::RenderMode Object::getRenderMode() const
{
	return renderMode;
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

Object* Object::copy() const
{
	Object* obj = new Object();
	obj->transformCache = transformCache;
	obj->transformed = transformed;
	obj->position = position;
	obj->scale = scale;
	obj->basis = basis;
	obj->parent = parent;
	obj->tags = tags;
	obj->renderMode = renderMode;

	for (auto child : children)
		obj->children.push_back(child->copy());

	return obj;
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

const glm::vec3& Object::getPosition() const
{
	return position;
}

const glm::vec3& Object::getScale() const
{
	return scale;
}

const glm::mat3& Object::getBasis() const
{
	return basis;
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

void Object::setBasis(const glm::mat3& basis)
{
	this->basis = basis;
	setTransformed();
}

void Object::rotate(const glm::vec3& axis, float angle)
{
	basis = glm::mat3(glm::rotate(angle, axis)) * basis;
	setTransformed();
}

void Object::addTag(const std::string& tag)
{
	tags.insert(tag);
}

bool Object::hasTag(const std::string& tag) const
{
	return tags.find(tag) != tags.end();
}

void Object::setTransformed()
{
	for (auto child : children)
		child->setTransformed();
	transformed = true;
}

void Object::ObjectRef::sort(std::list<ObjectRef>& objs)
{
	objs.sort([](const Object::ObjectRef& ref0, const Object::ObjectRef& ref1) { return ref0.dist < ref1.dist; });
}