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
	
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

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

	for (int x = cameraChunk.x - ext; x <= cameraChunk.x + ext; x++)
		for (int y = cameraChunk.y - ext; y <= cameraChunk.y + ext; y++)
		{
			if (chunks.find(glm::ivec2(x, y)) == chunks.end())
				chunks[glm::ivec2(x, y)] = generateChunk(glm::ivec2(x, y));
		}

	for (auto it = chunks.begin(); it != chunks.end(); it++)
	{
		auto chunk = it->second;

		glm::vec2 chunkCenter = chunk->pos + 0.5f * chunkSize;
		float chunkDist = glm::length(cameraPos - (glm::vec3(chunkCenter.x, 0.0f, chunkCenter.y)));
		int targetLod = static_cast<int>(glm::floor(glm::log2(chunkDist / chunkSize)));
		targetLod = glm::min(maxLod, glm::max(0, targetLod));

		if (it->first.x < cameraChunk.x - ext || it->first.x > cameraChunk.x + ext
			|| it->first.y < cameraChunk.y - ext || it->first.y > cameraChunk.y + ext)
		{
			targetLod = -1;
		}

		if (chunk->lod != targetLod)
			requestChunkLod(it->first, targetLod);
	}

	chunkUpdateMtx.lock();
	chunkReadyMtx.lock();
	for (auto it = chunkReadyQueue.begin(); it != chunkReadyQueue.end();)
	{
		if (loadReadyChunk(it->first))
			it = chunkReadyQueue.erase(it);
		else
			it++;
	}
	chunkReadyMtx.unlock();
	chunkUpdateMtx.unlock();
}

void TesselationTest::chunkGenerator()
{
	while (!stopChunkGenerator)
	{
		ChunkUpdateEvent updateEvent;
		updateEvent.free = false;
		chunkUpdateMtx.lock();
		for (auto it = chunkUpdateQueue.begin(); it != chunkUpdateQueue.end(); it++)
			if (it->second.free)
			{
				updateEvent = it->second;
				chunkReadyMtx.lock();
				chunkReadyQueue[it->first] = updateEvent;
				chunkReadyQueue[it->first].free = false;
				chunkReadyMtx.unlock();
				chunkUpdateQueue.erase(it);
				break;
			}
		chunkUpdateMtx.unlock();
		
		if (!updateEvent.free)
		{
			// If there is no events to process sleep the thread
			// to prevent continuous locking
			using namespace std::chrono_literals;
			std::this_thread::sleep_for(10ms);
			continue;
		}

		// loadReadyChunk won't run for this chunk at the same time

		// Update chunk data
		Chunk* chunk = chunks[updateEvent.gridPos];

		// Target LOD 0 - maxLod or -1 for chunk deletion
		if (chunk->lod < updateEvent.targetLod && updateEvent.targetLod != -1)
		{
			assert(chunk->normalMaps.empty());
			for (int lod = chunk->lod; lod < updateEvent.targetLod; lod++)
			{
				chunk->lods[lod].clear();
				chunk->lods[lod].resize(0);
			}
		}
		else if (chunk->lod > updateEvent.targetLod && updateEvent.targetLod != -1)
		{
			for (int lod = chunk->lod - 1; lod >= updateEvent.targetLod; lod--)
			{
				const auto& prev = lod == maxLod ? std::vector<float>() : chunk->lods[lod + 1];
				chunk->lods[lod] = std::move(generateChunkData(chunk->gridPos, lod, prev));
				chunk->normalMaps.push(std::move(generateNormals(chunk->lods[lod], lod)));
			}
		};

		// Mark chunk as ready
		chunkReadyMtx.lock();
		chunkReadyQueue[updateEvent.gridPos].free = true;
		chunkReadyMtx.unlock();
	}
}

void TesselationTest::requestChunkLod(const glm::ivec2& gridPos, int targetLod)
{
	chunkUpdateMtx.lock();
	chunkReadyMtx.lock();

	ChunkUpdateEvent updateEvent;

	updateEvent.gridPos = gridPos;
	updateEvent.targetLod = targetLod;

	if (chunkReadyQueue.find(gridPos) != chunkReadyQueue.end())
	{
		if (updateEvent.targetLod != chunkReadyQueue[gridPos].targetLod
			&& chunkReadyQueue[gridPos].targetLod != -1)
		{
			updateEvent.free = false;
			chunkUpdateQueue[gridPos] = updateEvent;
		}
	}
	else
	{
		updateEvent.free = true;
		chunkUpdateQueue[gridPos] = updateEvent;
	}

	chunkReadyMtx.unlock();
	chunkUpdateMtx.unlock();
}

std::vector<float> TesselationTest::generateChunkData(const glm::ivec2& pos, int lod, const std::vector<float>& prev)
{
	// generate next lod map
	const float freq = baseFreq / static_cast<float>(glm::pow(2, lod));
	const float amp = baseAmp * static_cast<float>(glm::pow(2.0f, lod) / glm::pow(2.0f, maxLod));
	const int currentTexSize = baseTexSize / static_cast<int>(glm::pow(2, lod));

	const float texelSize = chunkSize / (currentTexSize - 2);
	const glm::vec2 sampleStart = freq * (glm::vec2(pos) * chunkSize - texelSize / 2.0f);
	const float sampleWidth = freq * (texelSize);

	std::vector<float> data;
	data.reserve(currentTexSize * currentTexSize);
	for (int y = 0; y < currentTexSize; y++)
		for (int x = 0; x < currentTexSize; x++)
			data.push_back(Simplex::simplex2D(sampleStart + sampleWidth * glm::vec2(x, y), static_cast<int>(std::hash<int>()(lod))));

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

std::vector<glm::vec3> TesselationTest::generateNormals(const std::vector<float>& map, int lod)
{
	std::vector<glm::vec3> normals;
	normals.reserve(map.size());

	const int size = baseTexSize / static_cast<int>(glm::pow(2, lod));
	const float texelSize = chunkSize / (size - 2);

	for (int y = 0; y < size; y++)
	{
		const int yp = (y == size - 1 || y == 1) ? y : y + 1;
		const int ym = (y == 0 || y == size - 2) ? y : y - 1;

		for (int x = 0; x < size; x++)
		{
			const int xp = (x == size - 1 || x == 1) ? x : x + 1;
			const int xm = (x == 0 || x == size - 2) ? x : x - 1;

			const float dxm = map[static_cast<size_t>(xm + y * size)];
			const float dxp = map[static_cast<size_t>(xp + y * size)];
			const float dym = map[static_cast<size_t>(x + ym * size)];
			const float dyp = map[static_cast<size_t>(x + yp * size)];

			const float dx = dxp - dxm;
			const float dy = dyp - dym;

			const glm::vec3 normal = glm::normalize(glm::cross(
				glm::vec3(0, dy, texelSize * (yp - ym)),
				glm::vec3(texelSize * (xp - xm), dx, 0)));

			normals.push_back(normal / 2.0f + 0.5f);
		}
	}

	return normals;
}

bool TesselationTest::loadReadyChunk(const glm::ivec2& gridPos)
{
	if (!chunkReadyQueue[gridPos].free)
		return false;

	ChunkUpdateEvent updateEvent = chunkReadyQueue[gridPos];

	if (chunkUpdateQueue.find(gridPos) != chunkUpdateQueue.end())
		chunkUpdateQueue[gridPos].free = true;

	Chunk* chunk = chunks[gridPos];

	// Check if chunk should be destroyed
	if (updateEvent.targetLod == -1)
	{
		destroyChunk(chunk);
		chunks.erase(gridPos);
		return true;
	}

	if (updateEvent.targetLod < chunk->lod)
	{
		glBindTexture(GL_TEXTURE_2D, chunk->noiseTex);
		for (int lod = chunk->lod - 1; lod >= updateEvent.targetLod; lod--)
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, lod);
			const int texSize = baseTexSize / static_cast<int>(glm::pow(2, lod));
			glTexImage2D(GL_TEXTURE_2D, lod, GL_R16F, texSize, texSize, NULL, GL_RED, GL_FLOAT, chunk->lods[lod].data());
		}

		glBindTexture(GL_TEXTURE_2D, chunk->normalTex);
		for (int lod = chunk->lod - 1; lod >= updateEvent.targetLod; lod--)
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, lod);
			const int texSize = baseTexSize / static_cast<int>(glm::pow(2, lod));
			glTexImage2D(GL_TEXTURE_2D, lod, GL_RGB, texSize, texSize, NULL, GL_RGB, GL_FLOAT, chunk->normalMaps.front().data());
			chunk->normalMaps.pop();
		}
	}
	else if (updateEvent.targetLod > chunk->lod)
	{
		glBindTexture(GL_TEXTURE_2D, chunk->noiseTex);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, updateEvent.targetLod);
		glBindTexture(GL_TEXTURE_2D, chunk->normalTex);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, updateEvent.targetLod);

		for (int lod = chunk->lod; lod < updateEvent.targetLod; lod++)
		{
			glBindTexture(GL_TEXTURE_2D, chunk->noiseTex);
			glInvalidateTexImage(chunk->noiseTex, lod);
			glBindTexture(GL_TEXTURE_2D, chunk->normalTex);
			glInvalidateTexImage(chunk->normalTex, lod);
		}
	}
	glBindTexture(GL_TEXTURE_2D, 0);

	chunk->lod = updateEvent.targetLod;
	return true;
}

TesselationTest::Chunk* TesselationTest::generateChunk(const glm::ivec2 gridPos)
{
	Chunk* chunk = new Chunk();
	chunk->gridPos = gridPos;
	chunk->pos = chunkSize * glm::vec2(gridPos);
	chunk->lod = maxLod + 1;

	glGenTextures(1, &chunk->noiseTex);
	glBindTexture(GL_TEXTURE_2D, chunk->noiseTex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, maxLod);

	glGenTextures(1, &chunk->normalTex);
	glBindTexture(GL_TEXTURE_2D, chunk->normalTex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, maxLod);

	glBindTexture(GL_TEXTURE_2D, 0);

	return chunk;
}

void TesselationTest::renderChunk(const Chunk* chunk)
{
	if (chunk->lod == maxLod + 1)
		return;

	shader.set("uChunkPos", chunk->pos);
	shader.set("uChunkSize", chunkSize);
	shader.set("uNoiseTex", 0);
	shader.set("uNormalTex", 1);
	shader.set("uBaseLod", chunk->lod);
	shader.set("uMaxLod", maxLod);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, chunk->noiseTex);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, chunk->normalTex);

	glDrawArrays(GL_PATCHES, 0, verts);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void TesselationTest::destroyChunk(Chunk* chunk)
{
	glDeleteTextures(1, &chunk->noiseTex);
	glDeleteTextures(1, &chunk->normalTex);
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