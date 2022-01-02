#pragma once

#include "object.hpp"
#include "shader.hpp"

#include "glad/glad.h"
#include "glm/glm.hpp"

#include <unordered_set>

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

	std::list<VertexRef> selectVertices(const glm::vec2& start, const glm::vec2& size, const glm::mat4& projView) override;

	std::list<ObjectRef>selectObjects(const glm::vec2& screenCoord, const glm::mat4& projView, const glm::vec3& cameraPos) override;

	std::vector<glm::vec3> getVertices(const std::vector<unsigned>& indices) const;

	void deleteVertices(const std::vector<unsigned>& indices);

	static Mesh cube(Shader* shader);

	static Mesh cone(Shader* shader);

	static Mesh cylinder(Shader* shader);

	glm::vec3 position{};
	glm::vec3 scale{ 1.0f };

	void rotate(const glm::vec3& axis, float angle);

	glm::mat4 getTransform() const;

protected:
	glm::mat3 basis{1.0f};

	Shader* shader;

	std::vector<glm::vec3> vertices;
	std::list<std::vector<unsigned>> faces;

	struct uivec2hash { size_t operator()(const glm::uvec2& v) const { return std::hash<unsigned>()(v.x) ^ std::hash<unsigned>()(v.y); } };
	std::unordered_set<glm::uvec2, uivec2hash> edges;
	
	unsigned elements;

	GLuint vao;
	GLuint vbo;
	GLuint ebo;

	void addFaces(const std::vector<std::vector<unsigned>>& nFaces);

	void updateVertexBuffer();

	void updateElementBuffer();
};