#include "tesselation_test.hpp"
#include "simplex.hpp"

#include "GLFW\glfw3.h"

#include "glm\gtc\noise.hpp"
#include "glm\gtc\random.hpp"

#include <iostream>

TesselationTest::TesselationTest()
{
	std::vector<glm::vec3> vertices;
	for (int x = 1; x < 99; x++)
		for (int z = 1; z < 99; z++)
		{
			vertices.push_back(glm::vec3(0 + x, 0, 0 + z));
			vertices.push_back(glm::vec3(0 + x, 0, 1 + z));
			vertices.push_back(glm::vec3(1 + x, 0, 0 + z));
			vertices.push_back(glm::vec3(0 + x, 0, 1 + z));
			vertices.push_back(glm::vec3(1 + x, 0, 1 + z));
			vertices.push_back(glm::vec3(1 + x, 0, 0 + z));
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

	shader = Shader("shaders/tess/vtesstest.glsl", "shaders/tess/ctesstest.glsl",
		"shaders/tess/etesstest.glsl", "shaders/tess/ftesstest.glsl");

	glGenTextures(1, &noiseTex);
	glGenTextures(1, &noiseNormalTex);
	std::vector<float> pixels(texSize * texSize);

	Simplex::init();

	static const int OCTAVES = 10;
	static const float BASE_OCTAVE = 1024.0f;
	static const float BASE_MULT = 16.0f;
	float currentOctave = BASE_OCTAVE;
	float currentMult = BASE_MULT;
	float max = -FLT_MAX;
	float min = FLT_MAX;
	for (int oct = 0; oct < OCTAVES; oct++)
	{
		float freq = currentOctave + glm::linearRand(-currentOctave / 16.0f, currentOctave / 16.0f);

		int seed = std::hash<int>{}(oct);

		auto pixelsn = Simplex::simplex2Dgrid(glm::vec2(0), 1.0f / freq, texSize - 1, seed);

		for (int x = 0; x < texSize; x++)
			for (int y = 0; y < texSize; y++)
			{
				pixels[x + y * texSize] += currentMult * pixelsn[x + y * texSize];
				if (oct == OCTAVES - 1)
				{
					float val = pixels[x + y * texSize];
					min = glm::min(min, val);
					max = glm::max(max, val);
				}
			}

		currentMult /= 2.0f;
		currentOctave /= 2.0f;
	}

	for (int x = 0; x < texSize; x++)
		for (int y = 0; y < texSize; y++)
			pixels[x + y * texSize] = (pixels[x + y * texSize] - min) / (max - min);

	glBindTexture(GL_TEXTURE_2D, noiseTex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	glBindTexture(GL_TEXTURE_2D, noiseNormalTex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	// Generate mipmaps
	int size = texSize;
	std::vector<float> prevLevel = std::move(pixels);
	std::vector<float> nextLevel;
	int level = 0;
	while (true)
	{
		glBindTexture(GL_TEXTURE_2D, noiseTex);
		glTexImage2D(GL_TEXTURE_2D, level, GL_R16F, size, size, NULL, GL_RED, GL_FLOAT, prevLevel.data());

		// calculate normals
		std::vector<glm::vec3> normalPixels(size * size);
		float m = heightScale * size * texFreq;
		for (int x = 1; x < size - 1; x++)
			for (int y = 1; y < size - 1; y++)
			{
				float px = prevLevel[x + 1 + y * size];
				float mx = prevLevel[x - 1 + y * size];
				float pz = prevLevel[x + (y + 1) * size];
				float mz = prevLevel[x + (y - 1) * size];

				float pxpz = prevLevel[x + 1 + (y + 1) * size];
				float pxmz = prevLevel[x + 1 + (y - 1) * size];
				float mxmz = prevLevel[x - 1 + (y - 1) * size];
				float mxpz = prevLevel[x - 1 + (y + 1) * size];

				glm::vec3 dydx = glm::vec3(1.0f, (2.0f * px - 2.0f * mx + pxpz + pxmz - mxpz - mxmz) * m / 8.0f, 0.0f);
				glm::vec3 dydz = glm::vec3(0.0f, (2.0f * pz - 2.0f * mz + pxpz + mxpz - pxmz - mxmz) * m / 8.0f, 1.0f);

				glm::vec3 normal = glm::normalize(glm::cross(dydz, dydx));

				normalPixels[x + y * size] = normal / 2.0f + 0.5f;
			}
		glBindTexture(GL_TEXTURE_2D, noiseNormalTex);
		glTexImage2D(GL_TEXTURE_2D, level, GL_RGB, size, size, NULL, GL_RGB, GL_FLOAT, normalPixels.data());

		size /= 2;
		if (size < 1)
			break;

		nextLevel = std::vector<float>(size * size);

		for (int x = 0; x < size; x++)
			for (int y = 0; y < size; y++)
			{
				glm::vec4 s = glm::vec4(
					  prevLevel[2 * x + 2 * y * 2 * size]
					, prevLevel[(2 * x + 1) + 2 * y * 2 * size]
					, prevLevel[2 * x + (2 * y + 1) * 2 * size]
					, prevLevel[(2 * x + 1) + (2 * y + 1) * 2 * size]);

				float mean = 0.25f * (s.x + s.y + s.z + s.w);
				float min = glm::min(glm::min(s.x, s.y), glm::min(s.z, s.w));
				float max = glm::max(glm::max(s.x, s.y), glm::max(s.z, s.w));

				if (mean > 0.5f)
					nextLevel[x + y * size] = glm::mix(mean, max, (mean - 0.5f) / 0.5f);
				else
					nextLevel[x + y * size] = glm::mix(mean, min, (mean - 0.5f) / -0.5f);
			}

		prevLevel = std::move(nextLevel);
		level++;
	}

	glBindTexture(GL_TEXTURE_2D, 0);
}

void TesselationTest::render(const Camera* camera)
{
	shader.use();
	shader.set("uView", camera->getViewMat());
	shader.set("uProj", camera->getProjMat());
	shader.set("uCameraPos", camera->getPosition());
	shader.set("uNoiseTex", 0);
	shader.set("uNoiseNormalTex", 1);
	shader.set("uTextureFreq", texFreq);
	shader.set("uHeightScale", heightScale);

	glBindVertexArray(vao);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glEnable(GL_DEPTH_TEST);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, noiseTex);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, noiseNormalTex);

	glPatchParameteri(GL_PATCH_VERTICES, 3);

	glDrawArrays(GL_PATCHES, 0, verts);

	glBindVertexArray(0);
}