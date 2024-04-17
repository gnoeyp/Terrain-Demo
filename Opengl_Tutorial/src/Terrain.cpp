#include "Terrain.h"
#include "Texture.h"
#include <GL/glew.h>
#include <iostream>
#include <stb_image.h>
#include <vector>

unsigned int rez = 20;
unsigned int NUM_PATCH_PTS = 4;

glm::mat2x3 GetNormals(const std::vector<float>& vertices, int width, int height, int index)
{
	const unsigned int STRIDE = 3;
	int total = width * height;
	std::vector<glm::vec3> triangles;
	if (index > 0 && (index - 1) / width == index / width && index >= width)
		triangles.push_back(glm::vec3(index, index - 1, index - width));
	if (index / width == (index + 1) / width && index >= width)
		triangles.push_back(glm::vec3(index, index - width, index + 1));
	if (index > 0 && (index - 1) / width == index / width && index + width < total)
		triangles.push_back(glm::vec3(index, index + width, index - 1));
	if (index < total - 1 && (index + 1) / width == index / width && index + width < total)
		triangles.push_back(glm::vec3(index, index + 1, index + width));

	glm::vec3 tangent(0.0);
	glm::vec3 bitangent(0.0);
	glm::vec3 normal(0.0);
	glm::vec3 right(0.0);
	std::vector<glm::vec3> normals;
	std::vector<glm::vec3> tangents, bitangents;
	for (const glm::vec3& triangle : triangles)
	{
		glm::vec3 p0 = glm::vec3(vertices[STRIDE * triangle.x], vertices[STRIDE * triangle.x + 1], vertices[STRIDE * triangle.x + 2]);
		glm::vec3 p1 = glm::vec3(vertices[STRIDE * triangle.y], vertices[STRIDE * triangle.y + 1], vertices[STRIDE * triangle.y + 2]);
		glm::vec3 p2 = glm::vec3(vertices[STRIDE * triangle.z], vertices[STRIDE * triangle.z + 1], vertices[STRIDE * triangle.z + 2]);

		glm::vec3 a = p1 - p0;
		glm::vec3 b = p2 - p0;
		glm::vec3 c = p2 - p1;

		normal += normalize(glm::cross(b, a));

		if (a.x < 0) a.x *= -1;
		if (a.z < 0) a.z *= -1;
		if (b.x < 0) b.x *= -1;
		if (b.z < 0) b.z *= -1;
		if (c.x < 0) c.x *= -1;
		if (c.z < 0) c.z *= -1;

		if (a.z == 0)
			right += a;
		if (b.z == 0)
			right += b;
		if (c.z == 0)
			right += c;
	}

	normal /= (float)triangles.size();

	tangent = glm::normalize(glm::cross(normal, right));

	return glm::mat2x3(tangent, normal);
}

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

	//for (unsigned int i = 0; i < rez; i++)
	//{
	//	for (unsigned int j = 0; j < rez; j++)
	//	{

	//		glm::mat2x3 TB = GetNormals(positions, rez, rez, i * rez + j);
	//		tangents.push_back(TB[0][0]);
	//		tangents.push_back(TB[0][1]);
	//		tangents.push_back(TB[0][2]);
	//		bitangents.push_back(TB[1][0]);
	//		bitangents.push_back(TB[1][1]);
	//		bitangents.push_back(TB[1][2]);
	//	}
	//}

	//for (unsigned int i = 0; i < height; i++)
	//{
	//	for (unsigned int j = 0; j < width; j++)
	//	{
	//		unsigned char* texel = data + (j + width * i) * nChannels;
	//		unsigned char y = texel[0];

	//		positions.push_back(-height / 2.0f + i);
	//		positions.push_back((int)y * yScale - yShift);
	//		positions.push_back(-width / 2.0f + j);
	//	}
	//}

	//for (unsigned int i = 0; i < height; i++)
	//	for (unsigned int j = 0; j < width; j++)
	//	{
	//		textureCoords.push_back((float)j);
	//		textureCoords.push_back((float)i);
	//	}

	//for (unsigned int i = 0; i < height; i++)
	//{
	//	for (unsigned int j = 0; j < width; j++)
	//	{

	//		glm::mat2x3 TB = GetNormals(positions, width, height, i * width + j);
	//		tangents.push_back(TB[0][0]);
	//		tangents.push_back(TB[0][1]);
	//		tangents.push_back(TB[0][2]);
	//		bitangents.push_back(TB[1][0]);
	//		bitangents.push_back(TB[1][1]);
	//		bitangents.push_back(TB[1][2]);
	//	}
	//}

	//stbi_image_free(data);

	//std::vector<unsigned int> indices;

	//for (unsigned int i = 0; i < height - 1; i++)
	//{
	//	for (unsigned int j = 0; j < width; j++)
	//	{
	//		for (unsigned int k = 0; k < 2; k++)
	//		{
	//			indices.push_back(j + width * (i + k));
	//		}
	//	}
	//}

	//m_NumStrips = height - 1;
	//m_NumVertsPerStrip = width * 2;

	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);

	glGenBuffers(1, &m_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, (positions.size() + heightmapCoords.size() + textureCoords.size()) * sizeof(float), NULL, GL_STATIC_DRAW);
	//glBufferData(GL_ARRAY_BUFFER, (positions.size() + textureCoords.size() + tangents.size() + bitangents.size()) * sizeof(float), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, positions.size() * sizeof(float), &positions[0]);
	glBufferSubData(GL_ARRAY_BUFFER, positions.size() * sizeof(float), heightmapCoords.size() * sizeof(float), &heightmapCoords[0]);
	glBufferSubData(GL_ARRAY_BUFFER, (positions.size() + heightmapCoords.size()) * sizeof(float), textureCoords.size() * sizeof(float), &textureCoords[0]);
	//glBufferSubData(GL_ARRAY_BUFFER, (positions.size() + textureCoords.size()) * sizeof(float), tangents.size() * sizeof(float), &tangents[0]);
	//glBufferSubData(GL_ARRAY_BUFFER, (positions.size() + textureCoords.size() + tangents.size()) * sizeof(float), bitangents.size() * sizeof(float), &bitangents[0]);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)(positions.size() * sizeof(float)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)((positions.size() + heightmapCoords.size()) * sizeof(float)));
	glEnableVertexAttribArray(2);

	//glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)((positions.size() + textureCoords.size()) * sizeof(float)));
	//glEnableVertexAttribArray(2);

	//glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)((positions.size() + textureCoords.size() + tangents.size()) * sizeof(float)));
	//glEnableVertexAttribArray(3);

	//glGenBuffers(1, &m_EBO);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	glPatchParameteri(GL_PATCH_VERTICES, NUM_PATCH_PTS);

	//glGenTextures(1, &m_Tex);

	//int nChannels;

	//unsigned char* data = stbi_load(path, &width, &height, &nChannels, 0);
	//if (data)
	//{
	//	unsigned int format;
	//	if (nChannels == 1)
	//		format = GL_RED;
	//	else if (nChannels == 3)
	//		format = GL_RGB;
	//	else if (nChannels == 4)
	//		format = GL_RGBA;

	//	glBindTexture(GL_TEXTURE_2D, m_Tex);
	//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, format, GL_UNSIGNED_BYTE, data);
	//	glGenerateMipmap(GL_TEXTURE_2D);
	//}
	//stbi_image_free(data);

	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//glBindVertexArray(0);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	//glBindBuffer(GL_ARRAY_BUFFER, 0);


	// normal
	//glGenTextures(1, &m_Normal);
	//data = stbi_load(path, &width, &height, &nChannels, 0);
	//if (data)
	//{
	//	unsigned int format;
	//	if (nChannels == 1)
	//		format = GL_RED;
	//	else if (nChannels == 3)
	//		format = GL_RGB;
	//	else if (nChannels == 4)
	//		format = GL_RGBA;

	//	glBindTexture(GL_TEXTURE_2D, m_Normal);
	//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, format, GL_UNSIGNED_BYTE, data);
	//	glGenerateMipmap(GL_TEXTURE_2D);
	//}
	//stbi_image_free(data);

	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void Terrain::Draw(const Shader& shader) const
{
	glBindVertexArray(m_VAO);

	m_Heightmap.Bind(0);
	m_Texture.Bind(1);

	shader.Bind();

	shader.SetInt("u_Heightmap", 0);
	shader.SetInt("u_Texture", 1);
	shader.SetInt("u_NormalTexture", 2);

	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, m_Tex);
	//glActiveTexture(GL_TEXTURE1);
	//glBindTexture(GL_TEXTURE_2D, m_Normal);

	//shader.SetInt("u_Texture", 0);
	//shader.SetInt("u_NormalTexture", 1);

	glDrawArrays(GL_PATCHES, 0, NUM_PATCH_PTS * rez * rez);

	//for (unsigned int strip = 0; strip < m_NumStrips; ++strip)
	//{
	//	glDrawElements(GL_TRIANGLE_STRIP, m_NumVertsPerStrip, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * m_NumVertsPerStrip * strip));
	//}
}

