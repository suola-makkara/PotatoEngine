#pragma once

#include "shader.hpp"
#include "camera.hpp"
#include "object.hpp"
#include "ray.hpp"

#include "glad/glad.h"
#include "glm/glm.hpp"

#include <memory>

class EditorDrawUtils
{
public:
	EditorDrawUtils();
	EditorDrawUtils(const EditorDrawUtils&) = delete;
	EditorDrawUtils& operator=(const EditorDrawUtils&) = delete;

	~EditorDrawUtils();

	void drawSelection(const glm::vec2& start, const glm::vec2& size);

	void drawVertices(const std::vector<glm::vec3>& vertices, const glm::mat4& projView);

	void drawSelector(const glm::vec3& pos, const Camera* camera);

private:
	Shader selectionShader;
	GLuint selectionVao;
	GLuint selectionVbo;

	Shader vertexShader;
	GLuint vertexVao;
	GLuint vertexVbo;

	std::unique_ptr<Object> selector;
};