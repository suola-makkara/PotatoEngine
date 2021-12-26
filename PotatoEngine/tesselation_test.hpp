#pragma once

#include "shader.hpp"
#include "camera.hpp"

#include "glad\glad.h"
#include "glm\glm.hpp"

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

	//GLuint noiseTex;
	//GLuint noiseNormalTex;
	// 
	//float texFreq = 1.0f / 100.0f;
	//float heightScale = 40.0f;

	int verts;

	struct Chunk
	{
		GLuint noiseTex;
		//GLuint noiseNormalTex;
		int lod;
		glm::ivec2 gridPos;
		glm::vec2 pos;
		std::vector<float> currentLod;
	};

	int baseTexSize = 256;
	int maxLod = 2;

	float chunkSize = 64.0f; // meters
	int chunkSubdivision = 128;

	float baseFreq = 1.0f / 4.0f;
	float baseAmp = 0.8f;

	std::vector<Chunk> chunks;

	void updateChunk(Chunk& chunk, int lod);

	Chunk generateChunk(const glm::ivec2 gridPos);

	void generateChunkLod(Chunk& chunk);

	void renderChunk(const Chunk& chunk);

	float interp(const glm::ivec2& pos, const std::vector<float>& tex, int texSize);
};