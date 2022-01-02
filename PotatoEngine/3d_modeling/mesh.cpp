#include "mesh.hpp"
#include "camera.hpp"

#include "glm/gtc/constants.hpp"
#include "glm/gtx/transform.hpp"

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
	//shader->set("uPosition", position);
	shader->set("uModel", getTransform());
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

	auto tr = projView * getTransform();
	for (int i = 0; i < vertices.size(); i++)
	{
		auto pos = tr * glm::vec4(vertices[i], 1.0f);
		pos /= pos.w;
		if (pos.x > start.x && pos.y > start.y && pos.x < start.x + size.x && pos.y < start.y + size.y)
			ref.vertexIndices.push_back(i);
	}

	auto verts = Object::selectVertices(start, size, projView);
	verts.push_back(std::move(ref));

	return verts;
}

std::list<Object::ObjectRef> Mesh::selectObjects(const glm::vec2& screenCoord, const glm::mat4& projView, const glm::vec3& cameraPos)
{
	auto tr = projView * getTransform();
	std::vector<glm::vec3> projVerts;
	for (const auto& vert : vertices)
	{ 
		auto v = tr * glm::vec4(vert, 1);
		projVerts.push_back(glm::vec3(v.x, v.y, v.z) / v.w);
	}

	float dist = FLT_MAX;
	for (const auto& face : faces)
	{
		int sgn = 0;
		for (int i = 0; i < face.size(); i++)
		{
			const glm::vec3& a = projVerts[face[i]];
			const glm::vec3& b = projVerts[face[(i + 1) % face.size()]];

			const glm::vec2 A = glm::vec2(a.x, a.y);
			const glm::vec2 B = glm::vec2(b.x, b.y);

			const glm::vec2 AB = B - A;
			const glm::vec2 AS = screenCoord - A;

			const float c = AB.x * AS.y - AB.y * AS.x;

			const int nsgn = c > 0 ? 1 : -1;

			if (sgn == 0)
				sgn = nsgn;
			else if (nsgn != sgn)
			{
				sgn = 0;
				break;
			}
		}

		if (sgn != 0)
		{
			float d = glm::length(glm::vec3(tr * glm::vec4(vertices[face[0]], 1)) - cameraPos);
			dist = d < dist ? d : dist;
		}
	}

	auto objs = Object::selectObjects(screenCoord, projView, cameraPos);

	if (dist != FLT_MAX)
	{
		ObjectRef ref;
		ref.object = this;
		ref.dist = dist;
		objs.push_back(ref);
	}
	
	return objs;
}

std::vector<glm::vec3> Mesh::getVertices(const std::vector<unsigned>& indices) const
{
	auto tr = getTransform();
	std::vector<glm::vec3> verts;
	for (auto i : indices)
		verts.push_back(tr * glm::vec4(vertices[i], 1));

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

Mesh Mesh::cone(Shader* shader)
{
	Mesh mesh(shader);

	static const int steps = 10;
	std::vector<std::vector<unsigned>> nFaces;
	glm::vec3 top = glm::vec3(0, 0.5f, 0);
	//glm::vec3 bottom = glm::vec3(0, -0.5f, 0);
	mesh.vertices.push_back(top);
	//mesh.vertices.push_back(bottom);
	std::vector<unsigned> bottomFace;
	for (int i = 0; i < steps; i++)
	{
		float x = 0.5f * glm::cos(glm::two_pi<float>() * i / steps);
		float y = 0.5f * glm::sin(glm::two_pi<float>() * i / steps);
		glm::vec3 pos = glm::vec3(x, -0.5f, y);
		mesh.vertices.push_back(pos);
		nFaces.push_back(std::vector<unsigned>{0, static_cast<unsigned>(i + 1 == steps ? 1 : i + 2), static_cast<unsigned>(i + 1)});
		bottomFace.push_back(i + 1);
		//nFaces.push_back(std::vector<unsigned>{1, static_cast<unsigned>(i + 2), static_cast<unsigned>(i + 1 == steps ? 2 : i + 3)});
	}
	nFaces.push_back(std::move(bottomFace));

	mesh.addFaces(nFaces);

	mesh.updateVertexBuffer();
	mesh.updateElementBuffer();

	return mesh;
}

Mesh Mesh::cylinder(Shader* shader)
{
	Mesh mesh(shader);

	static const int steps = 10;
	std::vector<std::vector<unsigned>> nFaces;
	std::vector<unsigned> topFace;
	std::vector<unsigned> bottomFace;
	for (int i = 0; i < steps; i++)
	{
		float x = 0.5f * glm::cos(glm::two_pi<float>() * i / steps);
		float y = 0.5f * glm::sin(glm::two_pi<float>() * i / steps);
		glm::vec3 posTop = glm::vec3(x, 0.5f, y);
		glm::vec3 posBottom = glm::vec3(x, -0.5f, y);
		mesh.vertices.push_back(posTop);
		mesh.vertices.push_back(posBottom);
		topFace.push_back(2 * i);
		bottomFace.push_back(2 * i + 1);
		nFaces.push_back(std::vector<unsigned>{
			static_cast<unsigned>(2 * i + 1), 
			static_cast<unsigned>(2 * i),
			static_cast<unsigned>(i + 1 == steps ? 0 : 2 * (i + 1)),
			static_cast<unsigned>(i + 1 == steps ? 1 : 2 * (i + 1) + 1)
		});
	}
	std::reverse(topFace.begin(), topFace.end());
	nFaces.push_back(std::move(topFace));
	nFaces.push_back(std::move(bottomFace));

	mesh.addFaces(nFaces);

	mesh.updateVertexBuffer();
	mesh.updateElementBuffer();

	return mesh;
}

void Mesh::rotate(const glm::vec3& axis, float angle)
{
	basis = glm::mat3(glm::rotate(angle, axis)) * basis;
}

glm::mat4 Mesh::getTransform() const
{
	auto temp = glm::translate(position) * glm::mat4(basis) * glm::scale(scale);
	return temp;
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