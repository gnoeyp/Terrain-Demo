#pragma once

#include "Camera.h"
#include "glm/matrix.hpp"
#include <string>

class Shader
{
private:
	unsigned int m_ID;
	unsigned int m_UniformBindingIndex;

	unsigned int CompileShader(unsigned int type, const std::string& source) const;
	std::string ParseShader(const char* path) const;
	unsigned int CreateShader(const char* vertPath, const char* fragPath, const char* tcsPath, const char* tesPath, const char* geomPath) const;

	Shader& operator=(const Shader&);
	Shader(const Shader&);
public:
	Shader(const char* vertPath, const char* fragPath, const char* tcsPath = nullptr, const char* tesPath = nullptr, const char* geomPath = nullptr);
	~Shader();

	static Shader* BASIC;
	static Shader* BASIC_TEXTURE;
	static Shader* HOUSE;
	static Shader* HEIGHTMAP;
	static Shader* HEIGHTMAP_NORMAL;
	static Shader* FIRE;
	static Shader* SKYBOX;

	void Bind() const;
	void Unbind() const;
	void SetMat4f(const char* name, glm::mat4 m) const;
	void SetVec3f(const char* name, glm::vec3 v) const;
	void SetVec3f(const char* name, float v0, float v1, float v2) const;
	void SetVec4f(const char* name, glm::vec4 v) const;
	void SetVec4f(const char* name, float v0, float v1, float v2, float v3) const;
	void SetInt(const char* name, int v) const;
	void SetFloat(const char* name, float v) const;
	int GetLocation(const char* name) const;
	inline unsigned int GetID() const { return m_ID; }
	void BindUniformBlock(const char* name);

	static void Init()
	{
		Shader::BASIC = new Shader("res/shaders/basic.vert", "res/shaders/basic.frag");
		Shader::BASIC_TEXTURE = new Shader("res/shaders/basic_texture.vert", "res/shaders/basic_texture.frag");
		Shader::HOUSE = new Shader("res/shaders/house.vert", "res/shaders/house.frag");
		Shader::HEIGHTMAP = new Shader("res/shaders/heightmap.vert", "res/shaders/heightmap.frag", "res/shaders/heightmap.tesc", "res/shaders/heightmap.tese");
		Shader::HEIGHTMAP_NORMAL = new Shader("res/shaders/heightmap.vert", "res/shaders/heightmap_norm.frag", "res/shaders/heightmap.tesc", "res/shaders/heightmap.tese", "res/shaders/heightmap_norm.geom");
		Shader::FIRE = new Shader("res/shaders/fire.vert", "res/shaders/fire.frag");
		Shader::SKYBOX = new Shader("res/shaders/skybox.vert", "res/shaders/skybox.frag");
	}
	static void Clear()
	{
		delete Shader::BASIC;
		delete Shader::BASIC_TEXTURE;
		delete Shader::HOUSE;
		delete Shader::HEIGHTMAP;
		delete Shader::HEIGHTMAP_NORMAL;
		delete Shader::FIRE;
		delete Shader::SKYBOX;
	}
};

