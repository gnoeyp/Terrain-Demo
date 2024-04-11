#include "Ground.h"
#include <GL/glew.h>
#include <iostream>
#include <stb_image.h>

Ground::Ground() : m_VAO(0), m_VBO(0), m_EBO(0)
{
	float positions[] = {
		-1.0f, 0.0f, -1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, -1.0f, 1.0f, 0.0f,
		1.0f, 0.0f, 1.0f, 1.0f, 1.0f
	};

	unsigned int indices[] = {
		0, 1, 3,
		3, 2, 0
	};

	unsigned int groundVAO, groundVBO, groundEBO;
	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);

	glGenBuffers(1, &m_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);

	glGenBuffers(1, &m_EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

	glGenTextures(1, &m_Tex);

	int width, height, nrChannels;

	unsigned char* data = stbi_load("res/textures/grass/grass01.jpg", &width, &height, &nrChannels, 0);
	if (data)
	{
		GLenum format;
		if (nrChannels == 1)
			format = GL_RED;
		else if (nrChannels == 3)
			format = GL_RGB;
		else if (nrChannels == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, m_Tex);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		stbi_image_free(data);
	}
	else
	{
		std::cout << "Grass tex failed to load: " << std::endl;
		stbi_image_free(data);
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

Ground::~Ground()
{
	glDeleteBuffers(1, &m_VBO);
	glDeleteBuffers(1, &m_EBO);
	glDeleteVertexArrays(1, &m_VAO);
}

void Ground::Draw(const Shader& shader) const
{
	shader.Bind();
	shader.SetVec4f("u_Color", 0.0, 1.0, 0.0, 1.0);
	shader.SetInt("u_Texture", 0);
	glBindVertexArray(m_VAO);
	glBindTexture(GL_TEXTURE_2D, m_Tex);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);
}

