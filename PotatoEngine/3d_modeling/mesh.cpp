#include "mesh.hpp"
#include "camera.hpp"

Mesh::Mesh(Shader* shader)
	: shader(shader)
{
	glGenBuffers(1, &vbo);
	glGenVertexArrays(1, &vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindVertexArray(vao);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

Mesh::Mesh(Mesh&& mesh) noexcept
{
	*this = std::move(mesh);
}

Mesh::~Mesh()
{
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
}

Mesh& Mesh::operator=(Mesh&& mesh) noexcept
{
	vertices = std::move(mesh.vertices);
	vbo = mesh.vbo;
	vao = mesh.vao;
	shader = mesh.shader;

	mesh.vbo = 0;
	mesh.vao = 0;
	mesh.shader = nullptr;

	return *this;
}

void Mesh::render(const Camera* camera) const
{
	shader->use();
	shader->set("uProjView", camera->getProjViewMat());
	shader->set("uPosition", position);

	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, vertices.size());
	glBindVertexArray(0);

	Object::render(camera);
}

Mesh Mesh::cube(Shader* shader)
{
	Mesh mesh(shader);

	static const std::vector<glm::vec3> data
	{
		{1.0f, 1.0f, 1.0f},
		{1.0f, -1.0f, 1.0f},
		{1.0f, 1.0f, -1.0f},
		{1.0f, -1.0f, 1.0f},
		{1.0f, -1.0f, -1.0f},
		{1.0f, 1.0f, -1.0f},

		{-1.0f, 1.0f, 1.0f},
		{-1.0f, 1.0f, -1.0f},
		{-1.0f, -1.0f, 1.0f},
		{-1.0f, -1.0f, 1.0f},
		{-1.0f, 1.0f, -1.0f},
		{-1.0f, -1.0f, -1.0f},

		{1.0f, 1.0f, 1.0f},
		{1.0f, 1.0f, -1.0f},
		{-1.0f, 1.0f, 1.0f},
		{-1.0f, 1.0f, 1.0f},
		{1.0f, 1.0f, -1.0f},
		{-1.0f, 1.0f, -1.0f},

		{1.0f, -1.0f, 1.0f},
		{-1.0f, -1.0f, 1.0f},
		{1.0f, -1.0f, -1.0f},
		{-1.0f, -1.0f, 1.0f},
		{-1.0f, -1.0f, -1.0f},
		{1.0f, -1.0f, -1.0f},

		{1.0f, 1.0f, 1.0f},
		{-1.0f, 1.0f, 1.0f},
		{1.0f, -1.0f, 1.0f},
		{-1.0f, 1.0f, 1.0f},
		{-1.0f, -1.0f, 1.0f},
		{1.0f, -1.0f, 1.0f},

		{1.0f, 1.0f, -1.0f},
		{1.0f, -1.0f, -1.0f},
		{-1.0f, 1.0f, -1.0f},
		{-1.0f, 1.0f, -1.0f},
		{1.0f, -1.0f, -1.0f},
		{-1.0f, -1.0f, -1.0f}
	};

	mesh.vertices.insert(mesh.vertices.begin(), data.begin(), data.end());

	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(glm::vec3), data.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	return mesh;
}