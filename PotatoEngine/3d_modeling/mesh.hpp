#pragma once

#include "object.hpp"
#include "shader.hpp"

#include "glad/glad.h"
#include "glm/glm.hpp"

#include <unordered_set>

class Mesh : public Object
{
public:
	Mesh(Shader* fillShader, Shader* wireframeShader);

protected:
	Mesh(const Mesh&);
	Mesh(Mesh&& mesh) noexcept;

	Mesh& operator=(const Mesh&);
	Mesh& operator=(Mesh&& mesh) noexcept;

public:
	~Mesh();

	void render(const Camera* camera) const override;

	std::list<VertexRef> selectVertices(const glm::vec2& start, const glm::vec2& size, const glm::mat4& projView) override;

	std::list<ObjectRef>selectObjects(const Ray& ray) override;

	std::unique_ptr<Object> copy() const override;

	void setRenderMode(RenderMode mode) override;

	std::vector<glm::vec3> getVertices(const std::vector<unsigned>& indices) const;

	void deleteVertices(const std::vector<unsigned>& indices);

	glm::vec3 getCenter() const;

	static std::unique_ptr<Mesh> cube(Shader* fillShader, Shader* wireframeShader);

	static std::unique_ptr<Mesh> cone(Shader* fillShader, Shader* wireframeShader);

	static std::unique_ptr<Mesh> cylinder(Shader* fillShader, Shader* wireframeShader);

	static const glm::vec3 BASE_COLOR;
	static const glm::vec3 SELECTED_COLOR;

	glm::vec3 color = BASE_COLOR;

protected:
	Shader* fillShader;
	Shader* wireframeShader;

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