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

	verts = vertices.size();

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

	chunks.push_back(generateChunk(glm::ivec2(0)));
	chunks.push_back(generateChunk(glm::ivec2(1, 0)));

	//std::vector<glm::vec3> vertices;
	//for (int x = 1; x < 99; x++)
	//	for (int z = 1; z < 99; z++)
	//	{
	//		vertices.push_back(glm::vec3(0 + x, 0, 0 + z));
	//		vertices.push_back(glm::vec3(0 + x, 0, 1 + z));
	//		vertices.push_back(glm::vec3(1 + x, 0, 0 + z));
	//		vertices.push_back(glm::vec3(0 + x, 0, 1 + z));
	//		vertices.push_back(glm::vec3(1 + x, 0, 1 + z));
	//		vertices.push_back(glm::vec3(1 + x, 0, 0 + z));
	//	}

	//verts = vertices.size();

	//glGenBuffers(1, &vbo);
	//glGenVertexArrays(1, &vao);
	//glBindVertexArray(vao);
	//glBindBuffer(GL_ARRAY_BUFFER, vbo);
	//glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);
	//glEnableVertexAttribArray(0);
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
	//glBindVertexArray(0);
	//glBindBuffer(GL_ARRAY_BUFFER, 0);

	//shader = Shader("shaders/tess/vtesstest.glsl", "shaders/tess/ctesstest.glsl",
	//	"shaders/tess/etesstest.glsl", "shaders/tess/ftesstest.glsl");

	//glGenTextures(1, &noiseTex);
	//glGenTextures(1, &noiseNormalTex);
	//std::vector<float> pixels(texSize * texSize);

	//Simplex::init();

	//static const int OCTAVES = 10;
	//static const float BASE_OCTAVE = 1024.0f;
	//static const float BASE_MULT = 16.0f;
	//float currentOctave = BASE_OCTAVE;
	//float currentMult = BASE_MULT;
	//float max = -FLT_MAX;
	//float min = FLT_MAX;
	//for (int oct = 0; oct < OCTAVES; oct++)
	//{
	//	float freq = currentOctave + glm::linearRand(-currentOctave / 16.0f, currentOctave / 16.0f);

	//	int seed = std::hash<int>{}(oct);

	//	auto pixelsn = Simplex::simplex2Dgrid(glm::vec2(0), 1.0f / freq, texSize - 1, seed);

	//	for (int x = 0; x < texSize; x++)
	//		for (int y = 0; y < texSize; y++)
	//		{
	//			pixels[x + y * texSize] += currentMult * pixelsn[x + y * texSize];
	//			if (oct == OCTAVES - 1)
	//			{
	//				float val = pixels[x + y * texSize];
	//				min = glm::min(min, val);
	//				max = glm::max(max, val);
	//			}
	//		}

	//	currentMult /= 2.0f;
	//	currentOctave /= 2.0f;
	//}

	//for (int x = 0; x < texSize; x++)
	//	for (int y = 0; y < texSize; y++)
	//		pixels[x + y * texSize] = (pixels[x + y * texSize] - min) / (max - min);

	//glBindTexture(GL_TEXTURE_2D, noiseTex);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	//glBindTexture(GL_TEXTURE_2D, noiseNormalTex);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

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

	//	size /= 2;
	//	if (size < 1)
	//		break;

	//	nextLevel = std::vector<float>(size * size);

	//	for (int x = 0; x < size; x++)
	//		for (int y = 0; y < size; y++)
	//		{
	//			glm::vec4 s = glm::vec4(
	//				  prevLevel[2 * x + 2 * y * 2 * size]
	//				, prevLevel[(2 * x + 1) + 2 * y * 2 * size]
	//				, prevLevel[2 * x + (2 * y + 1) * 2 * size]
	//				, prevLevel[(2 * x + 1) + (2 * y + 1) * 2 * size]);

	//			float mean = 0.25f * (s.x + s.y + s.z + s.w);
	//			float min = glm::min(glm::min(s.x, s.y), glm::min(s.z, s.w));
	//			float max = glm::max(glm::max(s.x, s.y), glm::max(s.z, s.w));

	//			if (mean > 0.5f)
	//				nextLevel[x + y * size] = glm::mix(mean, max, (mean - 0.5f) / 0.5f);
	//			else
	//				nextLevel[x + y * size] = glm::mix(mean, min, (mean - 0.5f) / -0.5f);
	//		}

	//	prevLevel = std::move(nextLevel);
	//	level++;
	//}

	//glBindTexture(GL_TEXTURE_2D, 0);
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
		renderChunk(chunk);

	glBindVertexArray(0);

	//shader.use();
	//shader.set("uView", camera->getViewMat());
	//shader.set("uProj", camera->getProjMat());
	//shader.set("uCameraPos", camera->getPosition());
	//shader.set("uNoiseTex", 0);
	//shader.set("uNoiseNormalTex", 1);
	//shader.set("uTextureFreq", texFreq);
	//shader.set("uHeightScale", heightScale);

	//glBindVertexArray(vao);

	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);

	//glEnable(GL_DEPTH_TEST);

	////glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, noiseTex);
	//glActiveTexture(GL_TEXTURE1);
	//glBindTexture(GL_TEXTURE_2D, noiseNormalTex);

	//glPatchParameteri(GL_PATCH_VERTICES, 3);

	//glDrawArrays(GL_PATCHES, 0, verts);

	//glBindVertexArray(0);
}

void TesselationTest::update(const Camera* camera)
{
	// TODO generate update delete chunks
}

void TesselationTest::updateChunk(Chunk& chunk, int lod)
{
	//if (chunk.lod == -1)
	//{
	//	glGenTextures(1, &chunk.noiseTex);
	//	glGenTextures(1, &chunk.noiseNormalTex);
	//	glBindTexture(GL_TEXTURE_2D, chunk.noiseTex);
	//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//	glBindTexture(GL_TEXTURE_2D, chunk.noiseNormalTex);
	//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//}
	//else if (lod > chunk.lod)
	//{
	//	// Invalidate mipmaps to free storage and exit
	//	for (int i = 0; i < lod; i++)
	//	{
	//		glBindTexture(GL_TEXTURE_2D, chunk.noiseTex);
	//		glInvalidateTexImage(GL_TEXTURE_2D, i);
	//		glBindTexture(GL_TEXTURE_2D, chunk.noiseNormalTex);
	//		glInvalidateTexImage(GL_TEXTURE_2D, i);
	//	}

	//	glBindTexture(GL_TEXTURE_2D, chunk.noiseTex);
	//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, lod);
	//	glBindTexture(GL_TEXTURE_2D, chunk.noiseNormalTex);
	//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, lod);

	//	return;
	//}

	//// Genrerate missing mipmaps
	//for (int i = lod; i < chunk.lod == -1 ? INT_MAX : chunk.lod; i++)
	//{
	//	int size = texSize / glm::pow(2, lod);

	//	auto data = Simplex::simplex2Dgrid(glm::vec2(0), 1.0f / 128.0f, size - 1, 0);
	//}
}

TesselationTest::Chunk TesselationTest::generateChunk(const glm::ivec2 gridPos)
{
	Chunk chunk;
	chunk.gridPos = gridPos;
	chunk.pos = chunkSize * glm::vec2(gridPos);

	glGenTextures(1, &chunk.noiseTex);
	glBindTexture(GL_TEXTURE_2D, chunk.noiseTex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, maxLod);

	while (chunk.lod != 1)
		generateChunkLod(chunk);

	return chunk;

	/*
	*  - texture size 5
	*  - texel size 0.2
	*  - chunk size 9m
	*  - iii ~ 9m
	*  - oiiio ~ 5 * 9m / 3 ~ 15m
	*  - texel size m ~ chunk size / (texture size - 2) = 3m
	*  - chunk pos (C) 0m 8m
	*  - sample start (S) ~ C - texel size m / 2 ~ (-1.5m 6.5m)
	*  - sample width ~ texel size m ~ 3m
	*  - sample count ~ texture size ~ 5
	*
	*  - spatial frequency 1 / 3
	*  - sample start f ~ sample start * spatial freq
	*  - sample width f ~ sample width * spatial freq
	*  --- --- --- --- ---
	* | o | o | o | o | o |
	*  --- --- --- --- ---
	* | o | i | i | i | o |
	*  --- --- --- --- ---
	* | o | i | i | i | o |
	*  --- --- --- --- ---
	* | o | i | i | i | o |
	*  ---C--- --- --- ---
	* | S | o | o | o | o |
	*  --- --- --- --- ---
	*/

	//static const float freq = 1.0f / 32.0f; // 1 ~ 1 feature / meter -> 1/32 ~ 1 feature / 32 meters
	//static const float texelSize = chunkSize / (baseTexSize - 2);
	//glm::vec2 sampleStart = freq * (chunk.pos - texelSize / 2.0f);
	//float sampleWidth = freq * (texelSize);

	//auto data = Simplex::simplex2Dgrid(sampleStart, sampleWidth, baseTexSize - 1, 0);



	//static const float freq = 1.0f / 32.0f; // 1 ~ 1 feature / meter -> 1/32 ~ 1 feature / 32 meters
	//static const float texelSize = chunkSize / (baseTexSize / 2 - 2);
	//glm::vec2 sampleStart = freq * (chunk.pos - texelSize / 2.0f);
	//float sampleWidth = freq * (texelSize);

	//auto data = Simplex::simplex2Dgrid(sampleStart, sampleWidth, baseTexSize / 2 - 1, 0);

	//std::vector<float> datan;
	//datan.reserve(baseTexSize * baseTexSize);

	//for (int y = 0; y < baseTexSize; y++)
	//	for (int x = 0; x < baseTexSize; x++)
	//		datan.push_back(interp(glm::ivec2(x, y), data, baseTexSize / 2));

	//glTexImage2D(GL_TEXTURE_2D, 0, GL_R16F, baseTexSize, baseTexSize, NULL, GL_RED, GL_FLOAT, datan.data());

	//glBindTexture(GL_TEXTURE_2D, 0);

	//return chunk;
}

void TesselationTest::generateChunkLod(Chunk& chunk)
{
	if (!chunk.currentLod.empty() && chunk.lod == 0)
		return;

	// generate next lod map
	const int nextLod = chunk.currentLod.empty() ? maxLod : chunk.lod - 1;

	const float freq = baseFreq / glm::pow(2, nextLod);
	const float amp = baseAmp * glm::pow(2, nextLod) / glm::pow(2, maxLod);
	const int currentTexSize = baseTexSize / glm::pow(2, nextLod);

	const float texelSize = chunkSize / (currentTexSize - 2);
	const glm::vec2 sampleStart = freq * (chunk.pos - texelSize / 2.0f);
	const float sampleWidth = freq * (texelSize);

	auto data = Simplex::simplex2Dgrid(sampleStart, sampleWidth, currentTexSize - 1, 0);


	for (int x = 0; x < currentTexSize; x++)
		for (int y = 0; y < currentTexSize; y++)
		{
			data[x + y * currentTexSize] *= amp;
			if (!chunk.currentLod.empty())
				data[x + y * currentTexSize] += interp(glm::ivec2(x, y), chunk.currentLod, currentTexSize / 2);
		}

	glBindTexture(GL_TEXTURE_2D, chunk.noiseTex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, nextLod);
	glTexImage2D(GL_TEXTURE_2D, nextLod, GL_R16F, currentTexSize, currentTexSize, NULL, GL_RED, GL_FLOAT, data.data());
	glBindTexture(GL_TEXTURE_2D, 0);

	chunk.currentLod = data;
	chunk.lod = nextLod;
}

void TesselationTest::renderChunk(const Chunk& chunk)
{
	shader.set("uChunkPos", chunk.pos);
	shader.set("uChunkSize", chunkSize);
	shader.set("uNoiseTex", 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, chunk.noiseTex);

	glDrawArrays(GL_PATCHES, 0, verts);
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
