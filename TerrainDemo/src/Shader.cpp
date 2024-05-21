#include "GLUtils.h"
#include "Shader.h"
#include <fstream>
#include <GL/glew.h>
#include <iostream>
#include <sstream>

Shader* Shader::BASIC = nullptr;
Shader* Shader::BASIC_TEXTURE = nullptr;
Shader* Shader::HOUSE = nullptr;
Shader* Shader::HEIGHTMAP = nullptr;
Shader* Shader::FIRE = nullptr;
Shader* Shader::SKYBOX = nullptr;
Shader* Shader::HEIGHTMAP_NORMAL = nullptr;

std::string Shader::ParseShader(const char* path) const
{
	std::fstream stream(path);
	std::stringstream ss;
	std::string line;

	while (getline(stream, line))
	{
		ss << line << "\n";
	}

	return ss.str();
}

unsigned int Shader::CreateShader(const char* vertPath, const char* fragPath, const char* tcsPath, const char* tesPath, const char* geomPath) const
{
	unsigned int vert, frag, tsc = 0, tes = 0, geom = 0;
	std::string vertSource = ParseShader(vertPath);
	std::string fragSource = ParseShader(fragPath);
	vert = CompileShader(GL_VERTEX_SHADER, vertSource);
	frag = CompileShader(GL_FRAGMENT_SHADER, fragSource);

	if (tcsPath != nullptr)
	{
		std::string source = ParseShader(tcsPath);
		tsc = CompileShader(GL_TESS_CONTROL_SHADER, source);
	}

	if (tesPath != nullptr)
	{
		std::string source = ParseShader(tesPath);
		tes = CompileShader(GL_TESS_EVALUATION_SHADER, source);
	}

	if (geomPath != nullptr)
	{
		std::string source = ParseShader(geomPath);
		geom = CompileShader(GL_GEOMETRY_SHADER, source);
	}

	unsigned int program = glCreateProgram();
	glAttachShader(program, vert);
	glAttachShader(program, frag);
	if (tsc)
		glAttachShader(program, tsc);
	if (tes)
		glAttachShader(program, tes);
	if (geom)
		glAttachShader(program, geom);

	glLinkProgram(program);
	glValidateProgram(program);

	glDeleteShader(vert);
	glDeleteShader(frag);
	if (tsc)
		glDeleteShader(tsc);
	if (tes)
		glDeleteShader(tes);
	if (geom)
		glDeleteShader(geom);

	return program;
}

void Shader::Bind() const
{
	// TODO: add assert
	if (m_ID == 0)
		std::cout << "WARNING: shader is not initialized" << std::endl;
	glUseProgram(m_ID);
}

void Shader::Unbind() const
{
	glUseProgram(m_ID);
}

void Shader::SetMat4f(const char* name, glm::mat4 m) const
{
	glUniformMatrix4fv(GetLocation(name), 1, GL_FALSE, &m[0][0]);
}

void Shader::SetVec3f(const char* name, glm::vec3 v) const
{
	glUniform3f(GetLocation(name), v.x, v.y, v.z);
}

void Shader::SetVec3f(const char* name, float v0, float v1, float v2) const
{
	glUniform3f(GetLocation(name), v0, v1, v2);
}

void Shader::SetVec4f(const char* name, glm::vec4 v) const
{
	glUniform4f(GetLocation(name), v.x, v.y, v.z, v.w);
}

void Shader::SetVec4f(const char* name, float v0, float v1, float v2, float v3) const
{
	glUniform4f(GetLocation(name), v0, v1, v2, v3);
}

void Shader::SetInt(const char* name, int v) const
{
	glUniform1i(GetLocation(name), v);
}

int Shader::GetLocation(const char* name) const
{
	int location = glGetUniformLocation(m_ID, name);
	return location;
}

void Shader::BindUniformBlock(const char* name)
{
	unsigned int index = glGetUniformBlockIndex(m_ID, name);
	glUniformBlockBinding(m_ID, index, m_UniformBindingIndex++);
}

void Shader::SetFloat(const char* name, float v) const
{
	GLCall(glUniform1f(GetLocation(name), v));
}

Shader::Shader(const char* vertPath, const char* fragPath, const char* tcsPath, const char* tesPath, const char* geomPath)
{
	m_ID = CreateShader(vertPath, fragPath, tcsPath, tesPath, geomPath);
}

Shader::~Shader()
{
	if (m_ID != 0)
	{
		glDeleteProgram(m_ID);
	}
}

std::string ShaderType(unsigned int type)
{
	switch (type)
	{
	case GL_VERTEX_SHADER:
		return "vertex";
	case GL_FRAGMENT_SHADER:
		return "fragment";
	case GL_TESS_CONTROL_SHADER:
		return "tesselation control";
	case GL_TESS_EVALUATION_SHADER:
		return "tesselation evaluation";
	default:
		ASSERT(false);
	}
}

unsigned int Shader::CompileShader(unsigned int type, const std::string& source) const
{
	unsigned int id = glCreateShader(type);
	const char* src = source.c_str();
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);

	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if (result != GL_TRUE)
	{
		char message[1024];
		glGetShaderInfoLog(id, 1024, nullptr, message);
		std::cout << "Failed to compile " << ShaderType(type) << " shader" << std::endl;
		std::cout << message << std::endl;
	}

	return id;
}
