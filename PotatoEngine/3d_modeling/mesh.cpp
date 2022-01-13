#include "mesh.hpp"
#include "camera.hpp"
#include "intersection_test.hpp"

#include "glm/gtc/constants.hpp"
#include "glm/gtx/transform.hpp"

#include <unordered_map>

Mesh::Mesh(Shader* fillShader, Shader* wireframeShader)
	: fillShader(fillShader), wireframeShader(wireframeShader), elements(0)
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
	Mesh::operator=(std::move(mesh));
}

Mesh::~Mesh()
{
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ebo);
	glDeleteVertexArrays(1, &vao);
}

Mesh& Mesh::operator=(Mesh&& mesh) noexcept
{
	Object::operator=(std::move(mesh));

	vertices = std::move(mesh.vertices);
	faces = std::move(mesh.faces);
	edges = std::move(mesh.edges);
	vbo = mesh.vbo;
	vao = mesh.vao;
	ebo = mesh.ebo;
	fillShader = mesh.fillShader;
	wireframeShader = mesh.wireframeShader;
	elements = mesh.elements;

	mesh.vbo = 0;
	mesh.vao = 0;
	mesh.ebo = 0;
	mesh.fillShader = nullptr;
	mesh.wireframeShader = nullptr;
	mesh.elements = 0;

	return *this;
}

void Mesh::render(const Camera* camera) const
{
	auto shader = renderMode == RenderMode::DEFAULT ? fillShader : wireframeShader;

	shader->use();
	shader->set("uProjView", camera->getProjViewMat());
	shader->set("uModel", getTransform());
	shader->set("uColor", glm::vec4(color, 1.0));

	glEnable(GL_DEPTH_TEST);

	if (renderMode == RenderMode::DEFAULT)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glBindVertexArray(vao);
	if (renderMode == RenderMode::WIRE_FRAME)
		glDrawElements(GL_LINES, elements, GL_UNSIGNED_INT, 0);
	else
		glDrawElements(GL_TRIANGLES, elements, GL_UNSIGNED_INT, 0);
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

std::list<Object::ObjectRef> Mesh::selectObjects(const Ray& ray)
{
	const auto& tr = getTransform();
	std::vector<glm::vec3> trVerts;
	for (const auto& vert : vertices)
	{ 
		auto v = tr * glm::vec4(vert, 1);
		trVerts.push_back(glm::vec3(v.x, v.y, v.z) / v.w);
	}

	float tmin = FLT_MAX;

	for (const auto& face : faces)
	{
		unsigned first = face[0];
		unsigned prev = face[1];
		for (int i = 2; i < face.size(); i++)
		{
			float t = IntersectionTest::intersect(ray, trVerts[first], trVerts[prev], trVerts[face[i]]);
			if (t < tmin)
				tmin = t;
			prev = face[i];
		}
	}

	auto objs = Object::selectObjects(ray);

	if (tmin != FLT_MAX)
	{
		ObjectRef ref{};
		ref.dist = tmin;
		ref.object = this;
		objs.push_back(ref);
	}

	return objs;
}

void Mesh::setRenderMode(RenderMode mode)
{
	Object::setRenderMode(mode);
	updateElementBuffer();
}

std::vector<glm::vec3> Mesh::getVertices(const std::vector<unsigned>& indices) const
{
	const auto& tr = getTransform();
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
			remap.push_back(static_cast<unsigned>(nVerts.size()));
			nVerts.push_back(vertices[i]);
		}
	}

	vertices = nVerts;

	if (vertices.size() == 0)
	{
		parent->remove(this);
		return;
	}

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

glm::vec3 Mesh::getCenter() const
{
	const auto& tr = getTransform();

	glm::vec3 max = glm::vec3(-FLT_MAX);
	glm::vec3 min = glm::vec3(FLT_MAX);

	for (const auto& v : vertices)
	{
		max = glm::max(v, max);
		min = glm::min(v, min);
	}

	auto center = (min + max) / 2.0f;
	return glm::vec3(tr * glm::vec4(center, 1.0f));
}

Mesh Mesh::cube(Shader* fillShader, Shader* wireframeShader)
{
	Mesh mesh(fillShader, wireframeShader);

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

Mesh Mesh::cone(Shader* fillShader, Shader* wireframeShader)
{
	Mesh mesh(fillShader, wireframeShader);

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

Mesh Mesh::cylinder(Shader* fillShader, Shader* wireframeShader)
{
	Mesh mesh(fillShader, wireframeShader);

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

void Mesh::addFaces(const std::vector<std::vector<unsigned>>& nFaces)
{
	faces.insert(faces.begin(), nFaces.begin(), nFaces.end());

	for (const auto& face : faces)
	{
		for (int i = 0; i < face.size(); i++)
		{
			int a = face[i];
			int b = face[static_cast<size_t>(i + 1) % face.size()];
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
	if (renderMode == RenderMode::DEFAULT)
	{
		for (const auto& face : faces)
		{
			unsigned first = face[0];
			unsigned prev = face[1];
			for (int i = 2; i < face.size(); i++)
			{
				indices.push_back(first);
				indices.push_back(prev);
				indices.push_back(face[i]);
				prev = face[i];
			}
		}
	}
	else
	{
		for (const auto& edge : edges)
		{
			indices.push_back(edge.x);
			indices.push_back(edge.y);
		}
	}

	elements = static_cast<unsigned>(indices.size());
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, elements * sizeof(unsigned), indices.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

const glm::vec3 Mesh::BASE_COLOR = glm::vec3(0.3f);
const glm::vec3 Mesh::SELECTED_COLOR = glm::vec3(0.0f, 0.0f, 0.6f);