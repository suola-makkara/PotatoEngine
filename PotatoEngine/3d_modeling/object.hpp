#pragma once

#include "ray.hpp"

#include "glm/glm.hpp"

#include <list>
#include <vector>
#include <string>
#include <unordered_set>
#include <memory>

class Camera;

class Object
{
public:
	enum class RenderMode
	{
		DEFAULT,
		WIRE_FRAME,
	};

	struct VertexRef
	{
		Object* object{};
		std::vector<unsigned> vertexIndices{};
	};

	struct ObjectRef
	{
		Object* object;
		float dist;

		static void sort(std::list<ObjectRef>& objs);
	};

	Object() = default;

protected:
	Object(const Object&);
	Object(Object&& obj) noexcept;

	Object& operator=(const Object&);
	Object& operator=(Object&& obj) noexcept;

public:
	virtual ~Object() = default;

	virtual void render(const Camera* camera) const;

	virtual std::list<VertexRef> selectVertices(const glm::vec2& start, const glm::vec2& size, const glm::mat4& projView);

	virtual std::list<ObjectRef> selectObjects(const Ray& ray);

	virtual void setRenderMode(RenderMode mode);
	RenderMode getRenderMode() const;

	void add(std::unique_ptr<Object> object);
	void remove(Object* object);

	virtual std::unique_ptr<Object> copy() const;

	const glm::mat4& getTransform() const;

	const glm::vec3& getPosition() const;
	const glm::vec3& getScale() const;
	const glm::mat3& getBasis() const;

	void setPosition(const glm::vec3& position);
	void setScale(const glm::vec3& position);
	void setBasis(const glm::mat3& basis);
	void rotate(const glm::vec3& axis, float angle);

	void addTag(const std::string& tag);
	bool hasTag(const std::string& tag) const;

protected:
	void setTransformed();

	mutable glm::mat4 transformCache{ 1.0f };
	mutable bool transformed = false;

	glm::vec3 position{};
	glm::vec3 scale{ 1.0f };
	glm::mat3 basis{ 1.0f };

	RenderMode renderMode = RenderMode::DEFAULT;

	Object* parent = nullptr;

	std::list<std::unique_ptr<Object>> children;

	std::unordered_set<std::string> tags;
};