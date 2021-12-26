#pragma once

#include "shader.hpp"
#include "camera.hpp"

#include "glad\glad.h"
#include "glm\glm.hpp"

#include <unordered_map>
#include <functional>
#include <stack>

class TesselationTest
{
public:
	TesselationTest();

	void render(const Camera* camera);

	void update(const Camera* camera);

	friend class App;

private:
	Shader shader;

	GLuint vao;
	GLuint vbo;

	int verts;

	struct Chunk
	{
		GLuint noiseTex{};
		int lod{};
		glm::ivec2 gridPos{};
		glm::vec2 pos{};
		std::stack<std::vector<float>> lods{};
	};

	int baseTexSize = 1024;
	int maxLod = 8;

	float chunkSize = 64.0f; // meters
	int chunkSubdivision = 64;

	float baseFreq = 2.0f / 1.0f;
	float baseAmp = 4.0f;

	struct ivec2hash { size_t operator()(const glm::ivec2& v) const { return std::hash<int>()(v.x) ^ std::hash<int>()(v.y); } };
	std::unordered_map<glm::ivec2, Chunk, ivec2hash> chunks;

	Chunk generateChunk(const glm::ivec2 gridPos);

	void generateChunkLod(Chunk& chunk);

	void renderChunk(const Chunk& chunk);

	void destroyChunk(Chunk& chunk);

	float interp(const glm::ivec2& pos, const std::vector<float>& tex, int texSize);
};