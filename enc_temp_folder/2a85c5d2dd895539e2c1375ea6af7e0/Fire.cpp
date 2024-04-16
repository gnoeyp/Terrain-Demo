#include "Fire.h"
#include "Shader.h"
#include "Texture.h"
#include <GL/glew.h>
#include <glm/matrix.hpp>
#include <iostream>
#include <random>
#include <stb_image.h>

const unsigned int MAX_PARTICLES = 1000;
int index = 0;
float offset = 0.1f;

std::random_device rd;

Fire::Fire()
	: m_Particles(), m_InstanceVBO(0), m_VBO(0), m_VAO(0), m_EBO(0), m_Gen(rd()),
	m_Texture("res/textures/circle.png", { GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR })
{
	std::vector<float> instanceAttributes;

	std::uniform_int_distribution<int> dis(-99, 99);
	std::uniform_int_distribution<int> disPositive(0, 99);
	while (m_Particles.size() < m_NumGen)
	{
		FireParticle particle(
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(
				dis(m_Gen) / 10000.0f,
				disPositive(m_Gen) / 10000.0f,
				dis(m_Gen) / 10000.0f
			)
		);
		m_Particles.push_back(particle);

		// position
		instanceAttributes.push_back(particle.GetPosition().x);
		instanceAttributes.push_back(particle.GetPosition().y);
		instanceAttributes.push_back(particle.GetPosition().z);
		// color
		instanceAttributes.push_back(1.0f);
		instanceAttributes.push_back(0.0f);
		instanceAttributes.push_back(0.0f);
	}

	float quadVertices[] = {
		// position            // texture
		-0.05f,  -0.05f, 0.0f,	0.0f, 0.0f,
		-0.05f,  0.05f, 0.0f,	0.0, 1.0f,
		0.05f,  -0.05f, 0.0f,	1.0f, 0.0f,
		0.05f,  0.05f, 0.0f,	1.0f, 1.0f
	};

	unsigned int indices[] = {
		0, 1, 2,
		1, 3, 2
	};

	glGenBuffers(1, &m_InstanceVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_InstanceVBO);
	glBufferData(GL_ARRAY_BUFFER, 2 * 100 * sizeof(glm::vec3), &instanceAttributes[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenVertexArrays(1, &m_VAO);
	glGenBuffers(1, &m_VBO);
	glGenBuffers(1, &m_EBO);
	glBindVertexArray(m_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glBindBuffer(GL_ARRAY_BUFFER, m_InstanceVBO);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glVertexAttribDivisor(2, 1);
	glVertexAttribDivisor(3, 1);


	//glGenTextures(1, &m_Tex);
	//int width, height, nChannels;
	//unsigned char* data = stbi_load("res/textures/circle.png", &width, &height, &nChannels, 0);
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
	//	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
	//	glGenerateMipmap(GL_TEXTURE_2D);
	//	stbi_image_free(data);
	//}
	//else
	//{
	//	std::cout << "Fire tex failed to load: " << std::endl;
	//	stbi_image_free(data);
	//}

	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void Fire::Update()
{
	std::uniform_int_distribution<int> dis(-99, 99);
	std::uniform_int_distribution<int> disPositive(0, 99);

	if (m_Particles.size() < MAX_PARTICLES)
	{
		m_Particles.push_back(GenerateFireParticle());
	}

	unsigned int total = 0;
	for (const auto& particle : m_Particles)
	{
		if (!particle.IsDead()) total++;
	}

	std::cout << total << std::endl;

	if (total < MAX_PARTICLES)
	{
		for (unsigned int i = 0; i < m_Particles.size(); i++)
		{
			if (m_Particles[i].IsDead())
			{
				m_Particles[i] = GenerateFireParticle();
			}
		}
	}
	for (auto& particle : m_Particles)
	{
		if (!particle.IsDead())
		{
			particle.AddSpeed(glm::vec3(0.0f, 0.0001f, 0.0f));
			particle.Update();
		}
	}
}

void Fire::Draw(const Camera& camera) const
{
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glDepthMask(GL_FALSE);

	m_Texture.Bind();
	//glActiveTexture(0);
	//glBindTexture(GL_TEXTURE_2D, m_Tex);

	Shader::FIRE->Bind();
	Shader::FIRE->SetInt("u_Texture", 0);
	glm::mat4 model = glm::mat4(1.0f);
	model[3][1] = 10.0f;

	Shader::FIRE->SetMat4f("u_Model", model);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	std::vector<float> vertices = {
		-0.05f,  -0.05f, 0.0f, 0.0f, 0.0f,
		-0.05f,  0.05f, 0.0f, 0.0f, 1.0f,
		0.05f,  -0.05f, 0.0f, 1.0f, 0.0f,
		0.05f,  0.05f, 0.0f, 1.0f, 1.0f
	};
	glm::vec3 z = glm::normalize(-camera.GetFront());
	glm::vec3 y = glm::normalize(camera.GetUp());
	glm::vec3 x = glm::normalize(glm::cross(z, y));

	glm::mat3 rot = glm::transpose(glm::mat3(x, y, z));

	for (unsigned int i = 0; i < 4; i++)
	{
		glm::vec3 v(vertices[i * 5], vertices[i * 5 + 1], vertices[i * 5 + 2]);
		v = rot * v;
		vertices[i * 5] = v.x;
		vertices[i * 5 + 1] = v.y;
		vertices[i * 5 + 2] = v.z;
	}

	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, m_InstanceVBO);
	for (unsigned int i = 0; i < m_Particles.size(); i++)
	{
		if (m_Particles[i].IsDead()) continue;
		glm::vec3 data = m_Particles[i].GetPosition();
		glm::vec3 color = CalcColor(m_Particles[i].GetLife());
		glBufferSubData(GL_ARRAY_BUFFER, i * 2 * sizeof(glm::vec3), sizeof(glm::vec3), &data[0]);
		glBufferSubData(GL_ARRAY_BUFFER, (i * 2 + 1) * sizeof(glm::vec3), sizeof(glm::vec3), &color[0]);
	}
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(m_VAO);
	glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, m_Particles.size());
	glBindVertexArray(0);

	m_Texture.Unbind();

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthMask(GL_TRUE);
}

FireParticle Fire::GenerateFireParticle()
{
	std::uniform_int_distribution<int> dis(-20, 20);
	std::uniform_int_distribution<int> disPositive(0, 20);
	return FireParticle(
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(
			dis(m_Gen) / 10000.0f,
			disPositive(m_Gen) / 10000.0f,
			dis(m_Gen) / 10000.0f
		)
	);
}

glm::vec3 Fire::CalcColor(float life) const
{
	glm::vec3 white(1.0f, 1.0f, 1.0f);
	glm::vec3 yellow(1.0f, 1.0f, 0.0f);
	glm::vec3 red(1.0f, 0.0f, 0.0f);

	if (life < 0.2)
	{
		float ratio = (0.2f - life) / 0.2f;
		return (1.0f - ratio) * white + ratio * yellow;
	}
	else if (life < 0.5)
	{
		float ratio = (life - 0.2f) / 0.3f;
		return (1.0f - ratio) * yellow + ratio * red;
	}
	else
		return red;
}
