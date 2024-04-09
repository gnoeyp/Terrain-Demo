#include "Mesh.h"
#include <GL/glew.h>

Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, const std::vector<Texture>& textures, const std::vector<Color>& colors)
	: m_Vertices(vertices), m_Indices(indices), m_Textures(textures), m_Colors(colors)
{
	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);

	glGenBuffers(1, &m_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &m_EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), &indices[0], GL_STATIC_DRAW);

	// vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	//vertex normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
	// vertex texture coords
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

	glBindVertexArray(0);
}

void Mesh::Draw(const Shader& shader) const
{
	unsigned int diffuseNr = 1;
	unsigned int specularNr = 1;
	for (unsigned int i = 0; i < m_Textures.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		unsigned int number = 0;
		std::string name = m_Textures[i].type;
		if (name == "texture_diffuse")
			number = diffuseNr++;
		else if (name == "texture_specular")
			number = specularNr++;
		shader.SetInt((name + std::to_string(number)).c_str(), i);
		glBindTexture(GL_TEXTURE_2D, m_Textures[i].id);
	}

	for (auto& color : m_Colors)
	{
		shader.SetVec3f(color.type.c_str(), color.Value);
	}

	glBindVertexArray(m_VAO);
	glDrawElements(GL_TRIANGLES, m_Indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glActiveTexture(GL_TEXTURE0);
}
