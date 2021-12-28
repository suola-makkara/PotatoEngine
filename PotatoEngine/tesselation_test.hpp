#pragma once

#include "shader.hpp"
#include "camera.hpp"

#include "glad\glad.h"
#include "glm\glm.hpp"

#include <unordered_map>
#include <functional>
#include <stack>
#include <mutex>
#include <queue>
#include <thread>

class TesselationTest
{
public:
	TesselationTest();

	~TesselationTest();

	void render(const Camera* camera);

	void update(const Camera* camera);

	friend class App;

private:
	Shader shader;

	GLuint vao;
	GLuint vbo;

	int verts;

	int baseTexSize = 1024;
	static const int maxLod = 8;

	struct Chunk
	{
		GLuint noiseTex{};
		GLuint normalTex{};
		int lod{};
		glm::ivec2 gridPos{};
		glm::vec2 pos{};
		std::vector<float> lods[maxLod + 1]{};
		std::queue<std::vector<glm::vec3>> normalMaps{};
	};

	struct ChunkUpdateEvent
	{
		glm::ivec2 gridPos{};
		int targetLod{};
		bool free{};
	};

	float chunkSize = 64.0f; // meters
	int chunkSubdivision = 64;

	float baseFreq = 1.0f / 0.5f;
	float baseAmp = 12.0f;

	struct ivec2hash { size_t operator()(const glm::ivec2& v) const { return std::hash<int>()(v.x) ^ std::hash<int>()(v.y); } };
	std::unordered_map<glm::ivec2, Chunk*, ivec2hash> chunks;

	std::mutex chunkUpdateMtx{};
	std::mutex chunkReadyMtx{};
	bool stopChunkGenerator = false;
	std::unordered_map<glm::ivec2, ChunkUpdateEvent, ivec2hash> chunkUpdateQueue{};
	std::unordered_map<glm::ivec2, ChunkUpdateEvent, ivec2hash> chunkReadyQueue{};
	std::thread chunkUpdateThread;

	void chunkGenerator();

	void requestChunkLod(const glm::ivec2& gridPos, int targetLod);

	std::vector<float> generateChunkData(const glm::ivec2& pos, int lod, const std::vector<float>& prev);

	std::vector<glm::vec3> generateNormals(const std::vector<float>& map, int lod);

	bool loadReadyChunk(const glm::ivec2& gridPos);

	Chunk* generateChunk(const glm::ivec2 gridPos);

	void renderChunk(const Chunk* chunk);

	void destroyChunk(Chunk* chunk);

	float interp(const glm::ivec2& pos, const std::vector<float>& tex, int texSize);
};