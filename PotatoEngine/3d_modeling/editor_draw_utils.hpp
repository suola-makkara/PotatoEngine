#pragma once

#include "shader.hpp"
#include "camera.hpp"
#include "object.hpp"
#include "ray.hpp"

#include "glad/glad.h"
#include "glm/glm.hpp"

class EditorDrawUtils
{
public:
	static void drawSelection(const glm::vec2& start, const glm::vec2& size);

	static void drawVertices(const std::vector<glm::vec3>& vertices, const glm::mat4& projView);

	static void drawSelector(const glm::vec3& pos, const Camera* camera);

	static bool pickSelector(const Ray& ray);

	static void updateSelector(const Ray& ray);

	static void init();

	static void deinit();

private:
	static Shader selectionShader;
	static GLuint selectionVao;
	static GLuint selectionVbo;

	static Shader vertexShader;
	static GLuint vertexVao;
	static GLuint vertexVbo;

	static Object* selector;
	static int selectorAxis;
};