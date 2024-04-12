#include "Terrain.h"
#include <GL/glew.h>
#include <stb_image.h>
#include <vector>

Terrain::Terrain()
{
	int width, height, nChannels;
	unsigned char* data = stbi_load("res/textures/iceland_heightmap.png",
		&width, &height, &nChannels, 0);

	std::vector<float> vertices;
	float yScale = 64.0f / 256.0f, yShift = 16.0f;

	for (unsigned int i = 0; i < height; i++)
	{
		for (unsigned int j = 0; j < width; j++)
		{
			unsigned char* texel = data + (j + width * i) * nChannels;
			unsigned char y = texel[0];

			vertices.push_back(-height / 2.0f + i);
			vertices.push_back((int)y * yScale - yShift);
			vertices.push_back(-width / 2.0f + j);

			vertices.push_back((float)(j % 2));
			vertices.push_back((float)(i % 2));
		}
	}
	stbi_image_free(data);

	std::vector<unsigned int> indices;

	for (unsigned int i = 0; i < height - 1; i++)
	{
		for (unsigned int j = 0; j < width; j++)
		{
			for (unsigned int k = 0; k < 2; k++)
			{
				indices.push_back(j + width * (i + k));
			}
		}
	}

	m_NumStrips = height - 1;
	m_NumVertsPerStrip = width * 2;

	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);

	glGenBuffers(1, &m_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glGenBuffers(1, &m_EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	glGenTextures(1, &m_Tex);

	data = stbi_load("res/textures/grass/grass01.jpg", &width, &height, &nChannels, 0);
	if (data)
	{
		unsigned int format;
		if (nChannels == 1)
			format = GL_RED;
		else if (nChannels == 3)
			format = GL_RGB;
		else if (nChannels == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, m_Tex);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	stbi_image_free(data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	// normal
}

void Terrain::Draw() const
{
	glBindVertexArray(m_VAO);
	glBindTexture(GL_TEXTURE_2D, m_Tex);

	for (unsigned int strip = 0; strip < m_NumStrips; ++strip)
	{
		glDrawElements(GL_TRIANGLE_STRIP, m_NumVertsPerStrip, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * m_NumVertsPerStrip * strip));
	}
}

