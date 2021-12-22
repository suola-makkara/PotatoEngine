#include "tesselation_test.hpp"

#include "glm\gtc\noise.hpp"

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

	static const int LAYERS = 5;
	static const float scales[LAYERS]{ 640.0f, 320.0f, 160.0f, 80.0f, 40.0f };
	float mult[LAYERS];
	float sum = 0;
	for (int i = 0; i < LAYERS; i++) sum += glm::pow(scales[i], 0.6f);
	for (int i = 0; i < LAYERS; i++) mult[i] = glm::pow(scales[i], 0.6f) / sum;

	glGenTextures(1, &noiseTex);
	glGenTextures(1, &noiseNormalTex);
	std::vector<float> pixels(texSize * texSize);
	for (int i = 0; i < LAYERS; i++)
		for (int x = 0; x < texSize; x++)
		{
			//float sx = x < 25 ? x / 25.0f : x >= texSize - 25 ? (texSize - x - 1) / 25.0f : 1.0f;
			for (int y = 0; y < texSize; y++)
			{
				//float sy = y < 25 ? y / 25.0f : y >= texSize - 25 ? (texSize - y - 1) / 25.0f : 1.0f;
				pixels[x + y * texSize] += mult[i] * ((glm::perlin(glm::vec3(x / scales[i], y / scales[i], i)) + 1.0f) / 2.0f);
			}
		}

	glBindTexture(GL_TEXTURE_2D, noiseTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R16F, texSize, texSize, NULL, GL_RED, GL_FLOAT, pixels.data());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	std::vector<glm::vec3> normalPixels(texSize * texSize);
	float m = heightScale * texSize * texFreq;
	for (int x = 1; x < texSize - 1; x++)
		for (int y = 1; y < texSize - 1; y++)
		{
			float px = pixels[x + 1 + y * texSize];
			float mx = pixels[x - 1 + y * texSize];
			float pz = pixels[x + (y + 1) * texSize];
			float mz = pixels[x + (y - 1) * texSize];

			float pxpz = pixels[x + 1 + (y + 1) * texSize];
			float pxmz = pixels[x + 1 + (y - 1) * texSize];
			float mxmz = pixels[x - 1 + (y - 1) * texSize];
			float mxpz = pixels[x - 1 + (y + 1) * texSize];

			glm::vec3 dydx = glm::vec3(1.0f, (2.0f * px - 2.0f * mx + pxpz + pxmz - mxpz - mxmz) * m / 8.0f, 0.0f);
			glm::vec3 dydz = glm::vec3(0.0f, (2.0f * pz - 2.0f * mz + pxpz + mxpz - pxmz - mxmz) * m / 8.0f, 1.0f);

			glm::vec3 normal = glm::normalize(glm::cross(dydz, dydx));

			normalPixels[x + y * texSize] = normal / 2.0f + 0.5f;
		}

	glBindTexture(GL_TEXTURE_2D, noiseNormalTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texSize, texSize, NULL, GL_RGB, GL_FLOAT, normalPixels.data());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

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