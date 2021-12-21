#include "tesselation_test.hpp"

TesselationTest::TesselationTest()
{
	std::vector<glm::vec3> vertices;
	for (int x = -50; x < 50; x++)
		for (int z = -50; z < 50; z++)
		{
			vertices.push_back(glm::vec3(0 + x, 0, 0 + z));
			vertices.push_back(glm::vec3(0 + x, 0, 1 + z));
			vertices.push_back(glm::vec3(1 + x, 0, 0 + z));
			vertices.push_back(glm::vec3(0 + x, 0, 1 + z));
			vertices.push_back(glm::vec3(1 + x, 0, 1 + z));
			vertices.push_back(glm::vec3(1 + x, 0, 0 + z));
		}

	verts = vertices.size();

	glGenBuffers(1, &vbo);
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	shader = Shader("shaders/tess/vtesstest.glsl", "shaders/tess/ctesstest.glsl",
		"shaders/tess/etesstest.glsl", "shaders/tess/ftesstest.glsl");
}

void TesselationTest::render(const Camera* camera)
{
	shader.use();
	shader.set("uView", camera->getViewMat());
	shader.set("uProj", camera->getProjMat());
	shader.set("uCameraPos", camera->getPosition());

	glBindVertexArray(vao);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glEnable(GL_DEPTH_TEST);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glDrawArrays(GL_PATCHES, 0, verts);

	glBindVertexArray(0);
}