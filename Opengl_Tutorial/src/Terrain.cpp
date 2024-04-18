#include "Terrain.h"
#include "Texture.h"
#include <GL/glew.h>
#include <iostream>
#include <stb_image.h>
#include <vector>

unsigned int rez = 20;
unsigned int NUM_PATCH_PTS = 4;

Terrain::Terrain(const char* heightmapPath, const char* texturePath, const char* normalPath)
	: m_Heightmap(heightmapPath, { GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR }),
	m_Texture(texturePath, { GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR }),
	m_Normalmap(normalPath, { GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR })
{
	std::vector<float> positions;
	std::vector<float> heightmapCoords;
	std::vector<float> textureCoords;
	std::vector<float> normals, tangents, bitangents;
	float yScale = 64.0f / 256.0f, yShift = 16.0f;

	int width = m_Heightmap.GetWidth();
	int height = m_Heightmap.GetHeight();

	for (unsigned int i = 0; i < rez; i++)
	{
		for (unsigned int j = 0; j < rez; j++)
		{
			positions.push_back(-width / 2.0f + width * i / (float)rez); // v.x
			positions.push_back(0.0f); // v.y
			positions.push_back(-height / 2.0f + height * j / (float)rez); // v.z

			positions.push_back(-width / 2.0f + width * (i + 1) / (float)rez); // v.x
			positions.push_back(0.0f); // v.y
			positions.push_back(-height / 2.0f + height * j / (float)rez); // v.z

			positions.push_back(-width / 2.0f + width * i / (float)rez); // v.x
			positions.push_back(0.0f); // v.y
			positions.push_back(-height / 2.0f + height * (j + 1) / (float)rez); // v.z

			positions.push_back(-width / 2.0f + width * (i + 1) / (float)rez); // v.x
			positions.push_back(0.0f); // v.y
			positions.push_back(-height / 2.0f + height * (j + 1) / (float)rez); // v.z

			heightmapCoords.push_back(i / (float)rez); // u
			heightmapCoords.push_back(j / (float)rez); // v

			heightmapCoords.push_back((i + 1) / (float)rez); // u
			heightmapCoords.push_back(j / (float)rez); // v

			heightmapCoords.push_back(i / (float)rez); // u
			heightmapCoords.push_back((j + 1) / (float)rez); // v

			heightmapCoords.push_back((i + 1) / (float)rez); // u
			heightmapCoords.push_back((j + 1) / (float)rez); // v


			textureCoords.push_back(i * width / (float)rez);
			textureCoords.push_back(j * height / (float)rez);

			textureCoords.push_back((i + 1) * width / (float)rez);
			textureCoords.push_back(j * height / (float)rez);

			textureCoords.push_back(i * width / (float)rez);
			textureCoords.push_back((j + 1) * height / (float)rez);

			textureCoords.push_back((i + 1) * width / (float)rez);
			textureCoords.push_back((j + 1) * height / (float)rez);
		}
	}

	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);

	glGenBuffers(1, &m_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, (positions.size() + heightmapCoords.size() + textureCoords.size()) * sizeof(float), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, positions.size() * sizeof(float), &positions[0]);
	glBufferSubData(GL_ARRAY_BUFFER, positions.size() * sizeof(float), heightmapCoords.size() * sizeof(float), &heightmapCoords[0]);
	glBufferSubData(GL_ARRAY_BUFFER, (positions.size() + heightmapCoords.size()) * sizeof(float), textureCoords.size() * sizeof(float), &textureCoords[0]);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)(positions.size() * sizeof(float)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)((positions.size() + heightmapCoords.size()) * sizeof(float)));
	glEnableVertexAttribArray(2);

	glPatchParameteri(GL_PATCH_VERTICES, NUM_PATCH_PTS);
}

void Terrain::Draw(const Shader& shader) const
{
	glBindVertexArray(m_VAO);

	m_Heightmap.Bind(0);
	m_Texture.Bind(1);
	m_Normalmap.Bind(2);

	shader.Bind();

	shader.SetInt("u_Heightmap", 0);
	shader.SetInt("u_Texture", 1);
	shader.SetInt("u_NormalTexture", 2);
	shader.SetFloat("u_TexelSizeU", 1.0f / (float)m_Heightmap.GetWidth());
	shader.SetFloat("u_TexelSizeV", 1.0f / (float)m_Heightmap.GetHeight());

	glDrawArrays(GL_PATCHES, 0, NUM_PATCH_PTS * rez * rez);
}

