#pragma once

#include "Camera.h"
#include "glm/matrix.hpp"
#include <string>

class Shader
{
private:
	unsigned int m_ID;
public:
	Shader(const char* vertPath, const char* fragPath);
	~Shader();

	static Shader* BASIC;
	static Shader* BASIC_TEXTURE;

	unsigned int CompileShader(unsigned int type, const std::string& source) const;
	std::string ParseShader(const char* path) const;
	unsigned int CreateShader(const char* vertPath, const char* fragPath) const;
	void Bind() const;
	void Unbind() const;
	void SetMat4f(const char* name, glm::mat4 m) const;
	void SetVec3f(const char* name, glm::vec3 v) const;
	void SetVec3f(const char* name, float v0, float v1, float v2) const;
	void SetVec4f(const char* name, glm::vec4 v) const;
	void SetVec4f(const char* name, float v0, float v1, float v2, float v3) const;
	void SetInt(const char* name, int v) const;
	int GetLocation(const char* name) const;

	static void Init()
	{
		Shader::BASIC = new Shader("res/shaders/basic.vert", "res/shaders/basic.frag");
		Shader::BASIC_TEXTURE = new Shader("res/shaders/basic_texture.vert", "res/shaders/basic_texture.frag");
	}
	static void Clear()
	{
		delete Shader::BASIC;
		delete Shader::BASIC_TEXTURE;
	}
};

