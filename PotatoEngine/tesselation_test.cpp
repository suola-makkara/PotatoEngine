#include "tesselation_test.hpp"
#include "simplex.hpp"

#include "GLFW\glfw3.h"

#include "glm\gtc\noise.hpp"
#include "glm\gtc\random.hpp"

#include <iostream>

TesselationTest::TesselationTest()
{
	shader = Shader("shaders/tess/vtesstest.glsl", "shaders/tess/ctesstest.glsl",
		"shaders/tess/etesstest.glsl", "shaders/tess/ftesstest.glsl");

	std::vector<glm::vec3> vertices;
	static const float chunkGridStep = chunkSize / chunkSubdivision;
	for (int x = 0; x < chunkSubdivision; x++)
		for (int z = 0; z < chunkSubdivision; z++)
		{
			vertices.push_back(glm::vec3(0 + x, 0, 0 + z) * chunkGridStep);
			vertices.push_back(glm::vec3(0 + x, 0, 1 + z) * chunkGridStep);
			vertices.push_back(glm::vec3(1 + x, 0, 0 + z) * chunkGridStep);
			vertices.push_back(glm::vec3(0 + x, 0, 1 + z) * chunkGridStep);
			vertices.push_back(glm::vec3(1 + x, 0, 1 + z) * chunkGridStep);
			vertices.push_back(glm::vec3(1 + x, 0, 0 + z) * chunkGridStep);
		}

	verts = static_cast<int>(vertices.size());

	glGenBuffers(1, &vbo);
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	Simplex::init();

	chunkUpdateThread = std::thread(&TesselationTest::chunkGenerator, this);

	/*
	
	//// Generate mipmaps
	//int size = texSize;
	//std::vector<float> prevLevel = std::move(pixels);
	//std::vector<float> nextLevel;
	//int level = 0;
	//while (true)
	//{
	//	glBindTexture(GL_TEXTURE_2D, noiseTex);
	//	glTexImage2D(GL_TEXTURE_2D, level, GL_R16F, size, size, NULL, GL_RED, GL_FLOAT, prevLevel.data());

	//	// calculate normals
	//	std::vector<glm::vec3> normalPixels(size * size);
	//	float m = heightScale * size * texFreq;
	//	for (int x = 1; x < size - 1; x++)
	//		for (int y = 1; y < size - 1; y++)
	//		{
	//			float px = prevLevel[x + 1 + y * size];
	//			float mx = prevLevel[x - 1 + y * size];
	//			float pz = prevLevel[x + (y + 1) * size];
	//			float mz = prevLevel[x + (y - 1) * size];

	//			float pxpz = prevLevel[x + 1 + (y + 1) * size];
	//			float pxmz = prevLevel[x + 1 + (y - 1) * size];
	//			float mxmz = prevLevel[x - 1 + (y - 1) * size];
	//			float mxpz = prevLevel[x - 1 + (y + 1) * size];

	//			glm::vec3 dydx = glm::vec3(1.0f, (2.0f * px - 2.0f * mx + pxpz + pxmz - mxpz - mxmz) * m / 8.0f, 0.0f);
	//			glm::vec3 dydz = glm::vec3(0.0f, (2.0f * pz - 2.0f * mz + pxpz + mxpz - pxmz - mxmz) * m / 8.0f, 1.0f);

	//			glm::vec3 normal = glm::normalize(glm::cross(dydz, dydx));

	//			normalPixels[x + y * size] = normal / 2.0f + 0.5f;
	//		}
	//	glBindTexture(GL_TEXTURE_2D, noiseNormalTex);
	//	glTexImage2D(GL_TEXTURE_2D, level, GL_RGB, size, size, NULL, GL_RGB, GL_FLOAT, normalPixels.data());

	//}

	//glBindTexture(GL_TEXTURE_2D, 0);
	*/
}

TesselationTest::~TesselationTest()
{
	stopChunkGenerator = true;
	chunkUpdateThread.join();

	for (auto& chunk : chunks)
		delete chunk.second;
}

void TesselationTest::render(const Camera* camera)
{
	shader.use();
	shader.set("uView", camera->getViewMat());
	shader.set("uProj", camera->getProjMat());
	shader.set("uCameraPos", camera->getPosition());

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glEnable(GL_DEPTH_TEST);

	glPatchParameteri(GL_PATCH_VERTICES, 3);

	glBindVertexArray(vao);

	for (const auto& chunk : chunks)
		renderChunk(chunk.second);

	glUseProgram(0);

	glBindVertexArray(0);
}

void TesselationTest::update(const Camera* camera)
{
	glm::vec3 cameraPos = camera->getPosition();

	glm::ivec2 cameraChunk = glm::ivec2(glm::floor(glm::vec2(cameraPos.x, cameraPos.z) / chunkSize));

	static const int ext = 8;

	chunkMtx.lock();

	for (int x = cameraChunk.x - ext; x <= cameraChunk.x + ext; x++)
		for (int y = cameraChunk.y - ext; y <= cameraChunk.y + ext; y++)
		{
			if (chunks.find(glm::ivec2(x, y)) == chunks.end())
				chunks[glm::ivec2(x, y)] = generateChunk(glm::ivec2(x, y));
		}

	for (auto it = chunks.begin(); it != chunks.end();)
	{
		auto chunk = it->second;

		glm::vec2 chunkCenter = chunk->pos + 0.5f * chunkSize;
		float chunkDist = glm::length(cameraPos - (glm::vec3(chunkCenter.x, 0.0f, chunkCenter.y)));
		int targetLod = static_cast<int>(glm::floor(chunkDist / (2.0f * chunkSize)));
		targetLod = glm::min(maxLod, glm::max(0, targetLod));

		if (it->first.x < cameraChunk.x - ext || it->first.x > cameraChunk.x + ext
			|| it->first.y < cameraChunk.y - ext || it->first.y > cameraChunk.y + ext)
		{
			targetLod = -1;
		}

		chunk->sync.mtx.lock();

		if (chunk->sync.targetLod != targetLod)
		{
			chunk->sync.targetLod = targetLod;
			chunk->sync.ready = false;
			chunkUpdateQueue.push(chunk->gridPos);
		}

		chunk->sync.mtx.unlock();

		if (chunk->sync.ready && !loadReadyChunk(chunk))
			it = chunks.erase(it);
		else
			it++;
	}

	chunkMtx.unlock();
}

void TesselationTest::chunkGenerator()
{
	while (true)
	{
		chunkMtx.lock();
		while (!chunkUpdateQueue.empty() && !stopChunkGenerator)
		{
			glm::ivec2 chunkPos = chunkUpdateQueue.front();
			chunkUpdateQueue.pop();
			if (chunks.find(chunkPos) == chunks.end())
				continue;

			Chunk* chunk = chunks[chunkPos];

			chunkMtx.unlock();

			chunk->sync.mtx.lock();
			int currentLod = chunk->sync.lodState;
			int targetLod = chunk->sync.targetLod;
			if (currentLod == targetLod || targetLod == -1)
			{
				chunk->sync.ready = true;
				chunk->sync.mtx.unlock();
				chunkMtx.lock();
				continue;
			}
			chunk->sync.mtx.unlock();

			if (targetLod > currentLod)
			{
				for (int lod = currentLod; lod < targetLod; lod++)
					chunk->lods[lod].clear();
			}
			else if (targetLod < currentLod)
			{
				for (int lod = currentLod - 1; lod >= targetLod; lod--)
				{
					const auto& prev = lod == maxLod ? std::vector<float>() : chunk->lods[lod + 1];
					chunk->lods[lod] = std::move(generateChunkData(chunk->gridPos, lod, prev));
				}
			}

			chunk->sync.lodState = targetLod;

			chunk->sync.mtx.lock();
			if (chunk->sync.targetLod == targetLod)
				chunk->sync.ready = true;
			chunk->sync.mtx.unlock();

			chunkMtx.lock();
		}
		chunkMtx.unlock();

		if (stopChunkGenerator)
			break;
	}
}

std::vector<float> TesselationTest::generateChunkData(const glm::ivec2& pos, int lod, const std::vector<float>& prev)
{
	// generate next lod map
	const float freq = baseFreq / static_cast<float>(glm::pow(2, lod));
	const float amp = baseAmp * static_cast<float>(glm::pow(1.7f, lod) / glm::pow(1.7f, maxLod));
	const int currentTexSize = baseTexSize / static_cast<int>(glm::pow(2, lod));

	const float texelSize = chunkSize / (currentTexSize - 2);
	const glm::vec2 sampleStart = freq * (glm::vec2(pos) * chunkSize - texelSize / 2.0f);
	const float sampleWidth = freq * (texelSize);

	std::vector<float> data;
	data.reserve(currentTexSize * currentTexSize);
	for (int y = 0; y < currentTexSize; y++)
		for (int x = 0; x < currentTexSize; x++)
			data.push_back(Simplex::simplex2D(sampleStart + sampleWidth * glm::vec2(x, y), 0));

	// interpolate previous map
	for (int x = 0; x < currentTexSize; x++)
		for (int y = 0; y < currentTexSize; y++)
		{
			data[x + y * currentTexSize] *= amp;
			if (!prev.empty())
				data[x + y * currentTexSize] += interp(glm::ivec2(x, y), prev, currentTexSize / 2);
		}

	return data;
}

bool TesselationTest::loadReadyChunk(Chunk* chunk)
{
	// Check if chunk should be destroyed
	if (chunk->sync.targetLod == -1)
	{
		destroyChunk(chunk);
		return false;
	}

	if (chunk->sync.targetLod < chunk->lod || chunk->lod == INT_MAX)
	{
		if (chunk->lod == INT_MAX) chunk->lod = maxLod + 1;

		glBindTexture(GL_TEXTURE_2D, chunk->noiseTex);
		for (int lod = chunk->lod - 1; lod >= chunk->sync.targetLod; lod--)
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, lod);
			const int texSize = baseTexSize / static_cast<int>(glm::pow(2, lod));
			glTexImage2D(GL_TEXTURE_2D, lod, GL_R16F, texSize, texSize, NULL, GL_RED, GL_FLOAT, chunk->lods[lod].data());
		}
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	else if (chunk->sync.targetLod > chunk->lod)
	{
		glBindTexture(GL_TEXTURE_2D, chunk->noiseTex);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, chunk->sync.targetLod);

		for (int lod = chunk->lod; lod < chunk->sync.targetLod; lod++)
			glInvalidateTexImage(chunk->noiseTex, lod);

		glBindTexture(GL_TEXTURE_2D, 0);
	}

	chunk->lod = chunk->sync.targetLod;
	return true;
}

TesselationTest::Chunk* TesselationTest::generateChunk(const glm::ivec2 gridPos)
{
	Chunk* chunk = new Chunk();
	chunk->gridPos = gridPos;
	chunk->pos = chunkSize * glm::vec2(gridPos);
	chunk->lod = INT_MAX;
	chunk->sync.targetLod = INT_MAX;
	chunk->sync.lodState = maxLod + 1;
	chunk->sync.ready = false;

	glGenTextures(1, &chunk->noiseTex);
	glBindTexture(GL_TEXTURE_2D, chunk->noiseTex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, maxLod);

	return chunk;
}

void TesselationTest::generateChunkLod(Chunk* chunk)
{
	/*
	if (!chunk->lods.empty() && chunk->lod == 0)
		return;

	// generate next lod map
	const int nextLod = chunk->lods.empty() ? maxLod : chunk->lod - 1;

	const float freq = baseFreq / static_cast<float>(glm::pow(2, nextLod));
	const float amp = baseAmp * static_cast<float>(glm::pow(1.7f, nextLod) / glm::pow(1.7f, maxLod));
	const int currentTexSize = baseTexSize / static_cast<int>(glm::pow(2, nextLod));

	const float texelSize = chunkSize / (currentTexSize - 2);
	const glm::vec2 sampleStart = freq * (chunk->pos - texelSize / 2.0f);
	const float sampleWidth = freq * (texelSize);

	std::vector<float> data;
	data.reserve(currentTexSize * currentTexSize);
	for (int y = 0; y < currentTexSize; y++)
		for (int x = 0; x < currentTexSize; x++)
			data.push_back(Simplex::simplex2D(sampleStart + sampleWidth * glm::vec2(x, y), 0));

	// interpolate previous map
	for (int x = 0; x < currentTexSize; x++)
		for (int y = 0; y < currentTexSize; y++)
		{
			data[x + y * currentTexSize] *= amp;
			if (!chunk->lods.empty())
				data[x + y * currentTexSize] += interp(glm::ivec2(x, y), chunk->lods.top(), currentTexSize / 2);
		}

	glBindTexture(GL_TEXTURE_2D, chunk->noiseTex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, nextLod);
	glTexImage2D(GL_TEXTURE_2D, nextLod, GL_R16F, currentTexSize, currentTexSize, NULL, GL_RED, GL_FLOAT, data.data());
	glBindTexture(GL_TEXTURE_2D, 0);

	chunk->lods.push(std::move(data));
	chunk->lod = nextLod;
	*/
}

void TesselationTest::renderChunk(const Chunk* chunk)
{
	if (chunk->lod == INT_MAX)
		return;

	shader.set("uChunkPos", chunk->pos);
	shader.set("uChunkSize", chunkSize);
	shader.set("uNoiseTex", 0);
	shader.set("uBaseLod", chunk->lod);
	shader.set("uMaxLod", maxLod);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, chunk->noiseTex);

	glDrawArrays(GL_PATCHES, 0, verts);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void TesselationTest::destroyChunk(Chunk* chunk)
{
	glDeleteTextures(1, &chunk->noiseTex);
	delete chunk;
}

float TesselationTest::interp(const glm::ivec2& pos, const std::vector<float>& tex, int texSize)
{
	const float A = (texSize - 2.0f) / (2.0f * texSize - 2.0f);
	const float B = texSize / (4.0f * texSize - 4.0f);

	const glm::vec2 texelPos = A * glm::vec2(pos) + B;

	const glm::ivec2 mxmy = glm::floor(texelPos);
	const glm::ivec2 pxpy = glm::ceil(texelPos);

	const glm::vec2 fp = texelPos - glm::vec2(mxmy);

	const float mxi = glm::mix(tex[mxmy.x + mxmy.y * texSize], tex[pxpy.x + mxmy.y * texSize], fp.x);
	const float pxi = glm::mix(tex[mxmy.x + pxpy.y * texSize], tex[pxpy.x + pxpy.y * texSize], fp.x);

	return glm::mix(mxi, pxi, fp.y);
}
