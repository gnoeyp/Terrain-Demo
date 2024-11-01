#pragma once

#include "Shader.h"
#include <glm/matrix.hpp>
#include <iostream>
#include <string>
#include <vector>

struct Vertex
{
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
};

struct MeshTexture
{
	unsigned int id;
	std::string type;
	std::string path;
};

class Mesh
{
public:
	Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, const std::vector<MeshTexture>& textures);
	~Mesh();
	void Draw(const Shader& shader) const;
	Mesh& operator=(const Mesh&);
	Mesh(const Mesh&);
private:
	unsigned int m_VAO, m_VBO, m_EBO;
	std::vector<Vertex> m_Vertices;
	std::vector<unsigned int> m_Indices;
	std::vector<MeshTexture> m_Textures;
};

