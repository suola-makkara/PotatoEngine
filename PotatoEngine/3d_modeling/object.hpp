#pragma once

#include "ray.hpp"

#include "glm/glm.hpp"

#include <list>
#include <vector>

class Camera;

class Object
{
public:
	struct VertexRef
	{
		Object* object;
		std::vector<unsigned> vertexIndices;
	};

	struct ObjectRef
	{
		Object* object;
		float dist;
	};

	Object() = default;

	Object(const Object&) = delete;
	Object(Object&& obj) noexcept;

	Object& operator=(const Object&) = delete;
	Object& operator=(Object&& obj) noexcept;

	virtual ~Object();

	virtual void render(const Camera* camera) const;

	virtual std::list<VertexRef> selectVertices(const glm::vec2& start, const glm::vec2& size, const glm::mat4& projView);

	virtual std::list<ObjectRef> selectObjects(const Ray& ray);

	void add(Object* object);

	void remove(Object* object);

	const glm::mat4& getTransform() const;

	void setPosition(const glm::vec3& position);
	void setScale(const glm::vec3& position);
	void rotate(const glm::vec3& axis, float angle);

protected:
	void setTransformed();

	mutable glm::mat4 transformCache{ 1.0f };
	mutable bool transformed = false;

	glm::vec3 position{};
	glm::vec3 scale{ 1.0f };
	glm::mat3 basis{ 1.0f };

	Object* parent = nullptr;

	std::list<Object*> children;
};