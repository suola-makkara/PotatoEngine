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

	std::vector<glm::vec3> selectVertices(const glm::vec2& start, const glm::vec2& size, const glm::mat4& projView) override;

	static Mesh cube(Shader* shader);

	glm::vec3 position{};

protected:
	Shader* shader;

	std::list<glm::vec3> vertices;

	GLuint vao;
	GLuint vbo;
};