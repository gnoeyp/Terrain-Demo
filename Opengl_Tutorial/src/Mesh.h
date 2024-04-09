#pragma once

#include "Shader.h"
#include <glm/matrix.hpp>
#include <string>
#include <vector>

struct Vertex
{
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
};

struct Texture
{
	unsigned int id;
	std::string type;
	std::string path;
};

struct Color
{
	std::string type;
	glm::vec3 Value;
};

class Mesh
{
public:
	std::vector<Vertex> m_Vertices;
	std::vector<unsigned int> m_Indices;
	std::vector<Texture> m_Textures;
	std::vector<Color> m_Colors;

	Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, const std::vector<Texture>& textures, const std::vector<Color>& color);
	void Draw(const Shader& shader) const;
private:
	unsigned int m_VAO, m_VBO, m_EBO;
};

