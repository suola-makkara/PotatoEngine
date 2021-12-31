#include "mesh.hpp"
#include "camera.hpp"

#include <unordered_map>

Mesh::Mesh(Shader* shader)
	: shader(shader), elements(0)
{
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);
	glGenVertexArrays(1, &vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindVertexArray(vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

Mesh::Mesh(Mesh&& mesh) noexcept
{
	*this = std::move(mesh);
}

Mesh::~Mesh()
{
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ebo);
	glDeleteVertexArrays(1, &vao);
}

Mesh& Mesh::operator=(Mesh&& mesh) noexcept
{
	vertices = std::move(mesh.vertices);
	faces = std::move(mesh.faces);
	edges = std::move(mesh.edges);
	vbo = mesh.vbo;
	vao = mesh.vao;
	ebo = mesh.ebo;
	shader = mesh.shader;
	elements = mesh.elements;

	mesh.vbo = 0;
	mesh.vao = 0;
	mesh.ebo = 0;
	mesh.shader = nullptr;
	mesh.elements = 0;

	return *this;
}

void Mesh::render(const Camera* camera) const
{
	shader->use();
	shader->set("uProjView", camera->getProjViewMat());
	shader->set("uPosition", position);
	shader->set("uColor", glm::vec4(0.4, 0.4, 0.4, 1.0));

	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glBindVertexArray(vao);
	//glDrawElements(GL_TRIANGLES, elements, GL_UNSIGNED_INT, 0);
	glDrawElements(GL_LINES, elements, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glUseProgram(0);

	Object::render(camera);
}

std::list<Object::VertexRef> Mesh::selectVertices(const glm::vec2& start, const glm::vec2& size, const glm::mat4& projView)
{
	VertexRef ref;
	ref.object = this;

	for (int i = 0; i < vertices.size(); i++)
	{
		auto pos = projView * glm::vec4(vertices[i] + position, 1.0f);
		pos /= pos.w;
		if (pos.x > start.x && pos.y > start.y && pos.x < start.x + size.x && pos.y < start.y + size.y)
			ref.vertexIndices.push_back(i);
	}

	auto verts = Object::selectVertices(start, size, projView);
	verts.push_back(std::move(ref));

	return verts;
}

std::vector<glm::vec3> Mesh::getVertices(const std::vector<unsigned>& indices) const
{
	std::vector<glm::vec3> verts;
	for (auto i : indices)
		verts.push_back(vertices[i] + position);

	return verts;
}

void Mesh::deleteVertices(const std::vector<unsigned>& indices)
{
	std::vector<unsigned> remap;
	std::vector<glm::vec3> nVerts;

	int ii = 0;
	for (int i = 0; i < vertices.size(); i++)
	{
		if (ii < indices.size() && indices[ii] == i)
		{
			remap.push_back(UINT_MAX);
			ii++;
		}
		else
		{
			remap.push_back(nVerts.size());
			nVerts.push_back(vertices[i]);
		}
	}

	vertices = nVerts;

	for (auto it = faces.begin(); it != faces.end();)
	{
		bool del = false;
		for (int i = 0; i < it->size(); i++)
		{
			if (remap[(*it)[i]] == UINT_MAX)
			{
				del = true;
				break;
			}
			else
			{
				(*it)[i] = remap[(*it)[i]];
			}
		}
		if (del)
			it = faces.erase(it);
		else
			it++;
	}

	std::unordered_set<glm::uvec2, uivec2hash> nEdges;

	for (auto it = edges.begin(); it != edges.end(); it++)
	{
		if (remap[it->x] != UINT_MAX && remap[it->y] != UINT_MAX)
			nEdges.insert(glm::uvec2(remap[it->x], remap[it->y]));
	}

	edges = std::move(nEdges);

	updateVertexBuffer();
	updateElementBuffer();
}

Mesh Mesh::cube(Shader* shader)
{
	Mesh mesh(shader);

	/*
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
	*/

	static const std::vector<glm::vec3> dataV
	{
		{1,1,1},
		{1,-1,1},
		{1,-1,-1},
		{1,1,-1},
		{-1,1,1},
		{-1,1,-1},
		{-1,-1,-1},
		{-1,-1,1},
	};

	static const std::vector<std::vector<unsigned>> dataF
	{
		{0, 1, 2, 3},
		{4, 5, 6, 7},
		{0, 3, 5, 4},
		{1, 7, 6, 2},
		{0, 4, 7, 1},
		{2, 6, 5, 3}
	};

	mesh.vertices.insert(mesh.vertices.begin(), dataV.begin(), dataV.end());
	mesh.addFaces(dataF);

	mesh.updateVertexBuffer();
	mesh.updateElementBuffer();

	return mesh;
}

void Mesh::addFaces(const std::vector<std::vector<unsigned>>& nFaces)
{
	faces.insert(faces.begin(), nFaces.begin(), nFaces.end());

	for (const auto face : faces)
	{
		for (int i = 0; i < face.size(); i++)
		{
			int a = face[i];
			int b = face[(i + 1) % face.size()];
			edges.insert(glm::uvec2(glm::min(a, b), glm::max(a, b)));
		}
	}
}

void Mesh::updateVertexBuffer()
{
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Mesh::updateElementBuffer()
{
	std::vector<unsigned> indices;
	//for (const auto& face : faces)
	//{
	//	unsigned first = face[0];
	//	unsigned prev = face[1];
	//	for (int i = 2; i < face.size(); i++)
	//	{
	//		indices.push_back(first);
	//		indices.push_back(prev);
	//		indices.push_back(face[i]);
	//		prev = face[i];
	//	}
	//}

	for (const auto& edge : edges)
	{
		indices.push_back(edge.x);
		indices.push_back(edge.y);
	}

	elements = indices.size();
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, elements * sizeof(unsigned), indices.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}