#pragma once

#include "glm/matrix.hpp"
#include <string>

class Shader
{
public:
	Shader(const char* vertPath, const char* fragPath);
	~Shader();
	unsigned int CompileShader(unsigned int type, const std::string& source) const;
	std::string ParseShader(const char* path) const;
	unsigned int CreateShader(const char* vertPath, const char* fragPath) const;
	void Bind() const;
	void Unbind() const;
	void SetMat4f(const char* name, glm::mat4 m) const;
	void SetVec3f(const char* name, glm::vec3 v) const;
	void SetVec3f(const char* name, float v0, float v1, float v2) const;
	void SetInt(const char* name, int v) const;
	int GetLocation(const char* name) const;
private:
	unsigned int m_ID;
};

