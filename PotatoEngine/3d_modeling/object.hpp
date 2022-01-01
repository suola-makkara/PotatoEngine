#pragma once

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

	virtual ~Object();

	virtual void render(const Camera* camera) const;

	virtual std::list<VertexRef> selectVertices(const glm::vec2& start, const glm::vec2& size, const glm::mat4& projView);

	virtual std::list<ObjectRef> selectObjects(const glm::vec2& screenCoord, const glm::mat4& projView, const glm::vec3& cameraPos);

	void add(Object* object);

protected:
	std::list<Object*> children;
};