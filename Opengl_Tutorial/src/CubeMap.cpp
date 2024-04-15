#include "CubeMap.h"
#include "GLUtils.h"
#include <GL/glew.h>
#include <iostream>
#include <stb_image.h>

float skyboxVertices[] = {
	// positions          
	-1.0f,  1.0f, -1.0f,
	-1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	-1.0f,  1.0f, -1.0f,
	 1.0f,  1.0f, -1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	 1.0f, -1.0f,  1.0f
};

unsigned int GetFormat(int nrChannels)
{
	unsigned int format;
	switch (nrChannels)
	{
	case 1:
		return GL_RED;
	case 3:
		return GL_RGB;
	case 4:
		return GL_RGBA;
	default:
		ASSERT(false);
	}
}

CubeMap::CubeMap(std::vector<std::string> faces) : m_ID(0), m_VAO(0), m_VBO(0)
{
	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);

	glGenBuffers(1, &m_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);


	glGenTextures(1, &m_ID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_ID);

	int width, height, nrChannels;

	stbi_set_flip_vertically_on_load(false);

	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			unsigned int format = GetFormat(nrChannels);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, GL_RGBA, width, height, 0, format, GL_UNSIGNED_BYTE, data
			);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	stbi_set_flip_vertically_on_load(true);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

CubeMap::CubeMap(const char* path) : m_ID(0), m_VAO(0), m_VBO(0)
{
	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);

	glGenBuffers(1, &m_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);


	glGenTextures(1, &m_ID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_ID);

	int width, height, nrChannels;

	stbi_set_flip_vertically_on_load(false);

	unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);
	int sWidth = width / 4;
	int sHeight = height / 3;

	int coords[6][2] = { {2 , 1}, {0, 1}, {1, 0}, {1, 2 }, { 1, 1 }, { 3 , 1 } };

	unsigned int format = GetFormat(nrChannels);

	unsigned char* sData = new unsigned char[sWidth * sHeight * nrChannels];
	for (unsigned int i = 0; i < 6; i++)
	{
		for (unsigned int y = 0; y < sHeight; y++)
		{
			for (unsigned int x = 0; x < sWidth; x++)
			{
				for (unsigned int c = 0; c < nrChannels; c++)
				{
					sData[(y * sWidth + x) * nrChannels + c] = data[((4 * sWidth) * (coords[i][1] * sHeight + y) + coords[i][0] * sWidth + x) * nrChannels + c];
				}
			}
		}
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
			0, GL_RGBA, sWidth, sHeight, 0, format, GL_UNSIGNED_BYTE, sData
		);
	}
	delete[] sData;
	stbi_image_free(data);

	stbi_set_flip_vertically_on_load(true);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

void CubeMap::Draw(const Shader& shader) const
{
	glDepthFunc(GL_LEQUAL);
	shader.Bind();
	shader.SetInt("texture1", 0);

	glBindVertexArray(m_VAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_ID);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glDepthFunc(GL_LESS);
}

