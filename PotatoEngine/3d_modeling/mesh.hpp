#pragma once

#include "object.hpp"
#include "shader.hpp"

#include "glad/glad.h"
#include "glm/glm.hpp"

class Mesh : public Object
{
public:
	Mesh(Shader* shader);
	Mesh(const Mesh&) = delete;
	Mesh(Mesh&& mesh) noexcept;

	~Mesh();

	Mesh& operator=(const Mesh&) = delete;
	Mesh& operator=(Mesh&& mesh) noexcept;

	void render(const Camera* camera) const override;

	static Mesh cube(Shader* shader);

	glm::vec3 position{};

protected:
	Shader* shader;

	std::list<glm::vec3> vertices;

	GLuint vao;
	GLuint vbo;
};