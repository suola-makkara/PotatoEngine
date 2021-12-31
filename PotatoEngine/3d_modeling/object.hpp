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

	virtual ~Object();

	virtual void render(const Camera* camera) const;

	virtual std::list<VertexRef> selectVertices(const glm::vec2& start, const glm::vec2& size, const glm::mat4& projView);

	void add(Object* object);

protected:
	std::list<Object*> children;
};