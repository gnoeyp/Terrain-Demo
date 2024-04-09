#include "Shader.h"
#include <fstream>
#include <GL/glew.h>
#include <iostream>
#include <sstream>

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

unsigned int Shader::CreateShader(const char* vertPath, const char* fragPath) const
{
	std::string vertSource = ParseShader(vertPath);
	std::string fragSource = ParseShader(fragPath);
	unsigned int vert = CompileShader(GL_VERTEX_SHADER, vertSource);
	unsigned int frag = CompileShader(GL_FRAGMENT_SHADER, fragSource);

	unsigned int program = glCreateProgram();
	glAttachShader(program, vert);
	glAttachShader(program, frag);
	glLinkProgram(program);
	glValidateProgram(program);

	return program;
}

void Shader::Bind() const
{
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

void Shader::SetInt(const char* name, int v) const
{
	glUniform1i(GetLocation(name), v);
}

int Shader::GetLocation(const char* name) const
{
	int location = glGetUniformLocation(m_ID, name);
	return location;
}

Shader::Shader(const char* vertPath, const char* fragPath)
{
	m_ID = CreateShader(vertPath, fragPath);
}

Shader::~Shader()
{
	glDeleteProgram(m_ID);
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
		std::cout << "Failed to compile shader" << std::endl;
		std::cout << message << std::endl;
	}

	return id;
}
