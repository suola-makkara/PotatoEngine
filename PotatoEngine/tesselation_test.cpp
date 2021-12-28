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
				{
					chunk->lods[lod].clear();
					if (!chunk->normalMaps.empty())
						chunk->normalMaps.pop();
				}
			}
			else if (targetLod < currentLod)
			{
				for (int lod = currentLod - 1; lod >= targetLod; lod--)
				{
					const auto& prev = lod == maxLod ? std::vector<float>() : chunk->lods[lod + 1];
					chunk->lods[lod] = std::move(generateChunkData(chunk->gridPos, lod, prev));
					chunk->normalMaps.push(std::move(generateNormals(chunk->lods[lod], lod)));
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

		glBindTexture(GL_TEXTURE_2D, chunk->normalTex);
		for (int lod = chunk->lod - 1; lod >= chunk->sync.targetLod; lod--)
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, lod);
			const int texSize = baseTexSize / static_cast<int>(glm::pow(2, lod));
			glTexImage2D(GL_TEXTURE_2D, lod, GL_RGB, texSize, texSize, NULL, GL_RGB, GL_FLOAT, chunk->normalMaps.front().data());
			chunk->normalMaps.pop();
		}

		glBindTexture(GL_TEXTURE_2D, 0);
	}
	else if (chunk->sync.targetLod > chunk->lod)
	{
		glBindTexture(GL_TEXTURE_2D, chunk->noiseTex);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, chunk->sync.targetLod);
		glBindTexture(GL_TEXTURE_2D, chunk->normalTex);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, chunk->sync.targetLod);

		for (int lod = chunk->lod; lod < chunk->sync.targetLod; lod++)
		{
			glBindTexture(GL_TEXTURE_2D, chunk->noiseTex);
			glInvalidateTexImage(chunk->noiseTex, lod);
			glBindTexture(GL_TEXTURE_2D, chunk->normalTex);
			glInvalidateTexImage(chunk->normalTex, lod);
		}

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
	if (chunk->lod == INT_MAX)
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