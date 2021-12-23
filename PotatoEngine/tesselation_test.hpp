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

	friend class App;

private:
	Shader shader;

	GLuint vao;
	GLuint vbo;

	GLuint noiseTex;
	GLuint noiseNormalTex;

	int texSize = 1024;
	float texFreq = 1.0f / 100.0f;
	float heightScale = 40.0f;

	int verts;
};