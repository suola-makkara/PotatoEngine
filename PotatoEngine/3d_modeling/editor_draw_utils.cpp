#include "editor_draw_utils.hpp"
#include "editor_draw_utils.hpp"
#include "editor_draw_utils.hpp"
#include "mesh.hpp"

#include "glm/gtc/constants.hpp"

#include <iostream>

void EditorDrawUtils::drawSelection(const glm::vec2& start, const glm::vec2& size)
{
	selectionShader.use();
	selectionShader.set("uPosition", start);
	selectionShader.set("uSize", size);
	selectionShader.set("uColor", glm::vec4(0.7, 0.7, 0.9, 0.5));

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glBindVertexArray(selectionVao);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);

	glDisable(GL_BLEND);

	glUseProgram(0);
}

void EditorDrawUtils::drawVertices(const std::vector<glm::vec3>& vertices, const glm::mat4& projView)
{
	if (vertices.size() == 0)
		return;

	vertexShader.use();
	vertexShader.set("uProjView", projView);
	vertexShader.set("uModel", glm::mat4(1.0f));
	vertexShader.set("uColor", glm::vec4(0.7, 0.7, 0.9, 1.0));

	glBindBuffer(GL_ARRAY_BUFFER, vertexVbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glPointSize(4.0f);

	glBindVertexArray(vertexVao);
	glDrawArrays(GL_POINTS, 0, vertices.size());
	glBindVertexArray(0);

	glUseProgram(0);
}

void EditorDrawUtils::drawSelector(const glm::vec3& pos, const Camera* camera)
{
	selector->render(camera);
}

void EditorDrawUtils::init()
{
	selectionShader = Shader("shaders/vselect.glsl", "shaders/fcolor.glsl");

	static std::vector<glm::vec2> selectionData
	{ {0.0f, 0.0f}, {1.0f, 0.0f}, {0.0f, 1.0f}, {1.0f, 1.0f} };

	glGenVertexArrays(1, &selectionVao);
	glGenBuffers(1, &selectionVbo);
	glBindBuffer(GL_ARRAY_BUFFER, selectionVbo);
	glBindVertexArray(selectionVao);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * selectionData.size(), selectionData.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);


	vertexShader = Shader("shaders/vtest.glsl", "shaders/fcolor.glsl");

	glGenVertexArrays(1, &vertexVao);
	glGenBuffers(1, &vertexVbo);
	glBindBuffer(GL_ARRAY_BUFFER, vertexVbo);
	glBindVertexArray(vertexVao);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	selector = new Object();
	auto cyl = new Mesh(Mesh::cylinder(&vertexShader));
	cyl->setScale(glm::vec3(0.1f, 1.0f, 0.1f));
	cyl->color = glm::vec3(0, 1, 0);
	cyl->setPosition(glm::vec3(0, 0.5f, 0));
	selector->add(cyl);
	cyl = new Mesh(Mesh::cylinder(&vertexShader));
	cyl->setScale(glm::vec3(0.1f, 1.0f, 0.1f));
	cyl->rotate(glm::vec3(0, 0, 1), glm::half_pi<float>());
	cyl->color = glm::vec3(1, 0, 0);
	cyl->setPosition(glm::vec3(0.5f, 0, 0));
	selector->add(cyl);
	cyl = new Mesh(Mesh::cylinder(&vertexShader));
	cyl->setScale(glm::vec3(0.1f, 1.0f, 0.1f));
	cyl->rotate(glm::vec3(1, 0, 0), glm::half_pi<float>());
	cyl->color = glm::vec3(0, 0, 1);
	cyl->setPosition(glm::vec3(0, 0, 0.5f));
	selector->add(cyl);

	auto cone = new Mesh(Mesh::cone(&vertexShader));
	cone->setScale(glm::vec3(0.2f, 0.2f, 0.2f));
	cone->rotate(glm::vec3(1, 0, 0), glm::half_pi<float>());
	cone->color = glm::vec3(0, 0, 1);
	cone->setPosition(glm::vec3(0, 0, 1.0f));
	selector->add(cone);

	cone = new Mesh(Mesh::cone(&vertexShader));
	cone->setScale(glm::vec3(0.2f, 0.2f, 0.2f));
	cone->rotate(glm::vec3(0, 0, -1), glm::half_pi<float>());
	cone->color = glm::vec3(1, 0, 0);
	cone->setPosition(glm::vec3(1.0f, 0, 0));
	selector->add(cone);

	cone = new Mesh(Mesh::cone(&vertexShader));
	cone->setScale(glm::vec3(0.2f, 0.2f, 0.2f));
	cone->color = glm::vec3(0, 1, 0);
	cone->setPosition(glm::vec3(0, 1.0f, 0));
	selector->add(cone);
}

void EditorDrawUtils::deinit()
{
	glDeleteBuffers(1, &selectionVbo);
	glDeleteVertexArrays(1, &selectionVao);

	glDeleteBuffers(1, &vertexVbo);
	glDeleteVertexArrays(1, &vertexVao);

	delete selector;
}

Shader EditorDrawUtils::selectionShader = Shader();
GLuint EditorDrawUtils::selectionVao = 0;
GLuint EditorDrawUtils::selectionVbo = 0;

Shader EditorDrawUtils::vertexShader = Shader();
GLuint EditorDrawUtils::vertexVao = 0;
GLuint EditorDrawUtils::vertexVbo = 0;

Object* EditorDrawUtils::selector = nullptr;