#include "shader.hpp"

#include "glm\gtc\type_ptr.hpp"

#include <fstream>
#include <sstream>
#include <iostream>

Shader::Shader(Shader&& shader) noexcept
{
	for (int type = 0; type < SHADER_TYPES; type++)
		sourcesFiles[type] = std::move(shader.sourcesFiles[type]);

	programHandle = shader.programHandle;
	shader.programHandle = 0;
}

Shader& Shader::operator=(Shader&& shader) noexcept
{
	for (int type = 0; type < SHADER_TYPES; type++)
		sourcesFiles[type] = std::move(shader.sourcesFiles[type]);

	glDeleteProgram(programHandle);
	programHandle = shader.programHandle;
	shader.programHandle = 0;

	return *this;
}

Shader::Shader(const std::string& vertexShaderSourceFile, const std::string& fragmentShaderSourceFile)
{
	sourcesFiles[static_cast<int>(Type::VERTEX)] = vertexShaderSourceFile;
	sourcesFiles[static_cast<int>(Type::FRAGMENT)] = fragmentShaderSourceFile;

	compile();
}

Shader::Shader(const std::string& computeShaderSourceFile)
{
	sourcesFiles[static_cast<int>(Type::COMPUTE)] = computeShaderSourceFile;

	compile();
}

Shader::~Shader()
{
	glDeleteProgram(programHandle);
}

GLuint Shader::getProgram() const { return programHandle; }

bool Shader::isValid() const { return programHandle != 0; }

void Shader::use() { glUseProgram(programHandle); }

bool Shader::compile()
{
	GLuint newProgramHandle = glCreateProgram();
	GLuint shaderHandles[SHADER_TYPES]{};
	int status = 1;
	for (int type = 0; type < SHADER_TYPES; type++)
		if (!sourcesFiles[type].empty())
		{
			GLuint shaderHandle = compileShader(static_cast<Type>(type));
			if (shaderHandle != 0)
			{
				glAttachShader(newProgramHandle, shaderHandle);
				shaderHandles[type] = shaderHandle;
			}
			else
				status = 0;
		}

	glLinkProgram(newProgramHandle);
	for (int type = 0; type < SHADER_TYPES; type++)
		glDeleteShader(shaderHandles[type]);

	// Early exit if one or more of shader compilaions failed
	if (status == 0)
	{
		glDeleteProgram(newProgramHandle);
		return false;
	}

	glGetProgramiv(newProgramHandle, GL_LINK_STATUS, &status);
	if (!status)
	{
		static constexpr const int BUFFER_SIZE = 512;
		char log[BUFFER_SIZE];
		glGetProgramInfoLog(newProgramHandle, BUFFER_SIZE, NULL, log);
		std::cout << "Error in linking program:\n" << log;

		glDeleteProgram(newProgramHandle);
		return false;
	}
	else
	{
		glDeleteProgram(programHandle);
		programHandle = newProgramHandle;

		return true;
	}
}

void Shader::set(const std::string& uniform, float v) { glUniform1f(location(uniform), v); }
void Shader::set(const std::string& uniform, int i) { glUniform1i(location(uniform), i); };
void Shader::set(const std::string& uniform, const glm::vec2& v) { glUniform2fv(location(uniform), 1, glm::value_ptr(v)); };
void Shader::set(const std::string& uniform, const glm::vec3& v) { glUniform3fv(location(uniform), 1, glm::value_ptr(v)); };
void Shader::set(const std::string& uniform, const glm::vec4& v) { glUniform4fv(location(uniform), 1, glm::value_ptr(v)); };
void Shader::set(const std::string& uniform, const glm::mat2& m) { glUniformMatrix2fv(location(uniform), 1, GL_FALSE, glm::value_ptr(m)); };
void Shader::set(const std::string& uniform, const glm::mat3& m) { glUniformMatrix3fv(location(uniform), 1, GL_FALSE, glm::value_ptr(m)); };
void Shader::set(const std::string& uniform, const glm::mat4& m) { glUniformMatrix4fv(location(uniform), 1, GL_FALSE, glm::value_ptr(m)); };

GLint Shader::location(const std::string& uniform) const { return glGetUniformLocation(programHandle, uniform.c_str()); }

GLuint Shader::compileShader(Type type)
{
	// Read file
	const auto& sourceFile = sourcesFiles[static_cast<int>(type)];
	std::ifstream stream(sourceFile);
	std::stringstream ss;
	ss << stream.rdbuf();
	std::string source = ss.str();
	const char* sourcePtr = source.c_str();
	
	// Compile shader
	GLenum glType = 0;
	switch (type)
	{
	case Shader::Type::VERTEX:
		glType = GL_VERTEX_SHADER;
		break;
	case Shader::Type::GEOMETRY:
		glType = GL_GEOMETRY_SHADER;
		break;
	case Shader::Type::TESS_EVAL:
		glType = GL_TESS_EVALUATION_SHADER;
		break;
	case Shader::Type::TESS_CTRL:
		glType = GL_TESS_CONTROL_SHADER;
		break;
	case Shader::Type::FRAGMENT:
		glType = GL_FRAGMENT_SHADER;
		break;
	case Shader::Type::COMPUTE:
		glType = GL_COMPUTE_SHADER;
		break;
	}

	GLuint shaderHandle = glCreateShader(glType);
	glShaderSource(shaderHandle, 1, &sourcePtr, NULL);
	glCompileShader(shaderHandle);

	int status = 0;
	glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &status);
	if (!status)
	{
		static constexpr const int BUFFER_SIZE = 512;
		char log[BUFFER_SIZE];
		glGetShaderInfoLog(shaderHandle, BUFFER_SIZE, NULL, log);
		log[BUFFER_SIZE - 1] = '\0';
		std::cout << "Error in compiling " << shaderTypeNames[static_cast<int>(type)] << "_SHADER:\n" << log;

		glDeleteShader(shaderHandle);
		return 0;
	}

	return shaderHandle;
}

const std::string Shader::shaderTypeNames[]{ "VERTEX", "GEOMETRY", "TESSELATION_EVALUATION",
	"TESSELATION_CONTROL", "FRAGMENT", "COMPUTE" };