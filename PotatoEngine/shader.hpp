#pragma once

#include "glad\glad.h"
#include "glm\glm.hpp"

#include <vector>
#include <string>

class Shader
{
public:
	Shader() = default;

	Shader(const Shader&) = delete;
	Shader& operator=(const Shader&) = delete;

	Shader(Shader&& shader) noexcept;
	Shader& operator=(Shader&& shader) noexcept;

	Shader(const std::string& vertexShaderSourceFile, const std::string& fragmentShaderSourceFile);

	Shader(const std::string& vertexShaderSourceFile, const std::string& geometryShaderSourceFile,
		const std::string& fragmentShaderSourceFile);

	Shader(const std::string& vertexShaderSourceFile, const std::string& tessCtrlShaderSourceFile,
		const std::string& tessEvalShaderSourceFile, const std::string& fragmentShaderSourceFile);

	Shader(const std::string& computeShaderSourceFile);

	~Shader();

	GLuint getProgram() const;

	bool isValid() const;

	void use();

	bool compile();

	void set(const std::string& uniform, float v);
	void set(const std::string& uniform, int i);
	void set(const std::string& uniform, const glm::ivec2& v);
	void set(const std::string& uniform, const glm::ivec3& v);
	void set(const std::string& uniform, const glm::ivec4& v);
	void set(const std::string& uniform, const glm::vec2& v);
	void set(const std::string& uniform, const glm::vec3& v);
	void set(const std::string& uniform, const glm::vec4& v);
	void set(const std::string& uniform, const glm::mat2& m);
	void set(const std::string& uniform, const glm::mat3& m);
	void set(const std::string& uniform, const glm::mat4& m);

	GLint location(const std::string& uniform) const;

private:
	GLuint programHandle = 0;

	static const int SHADER_TYPES = 6;
	std::string sourcesFiles[SHADER_TYPES];

	static const std::string shaderTypeNames[SHADER_TYPES];

	enum class Type
	{
		VERTEX      = 0,
		GEOMETRY    = 1,
		TESS_CTRL   = 2,
		TESS_EVAL   = 3,
		FRAGMENT    = 4,
		COMPUTE     = 5
	};

	GLuint compileShader(Type type);
};