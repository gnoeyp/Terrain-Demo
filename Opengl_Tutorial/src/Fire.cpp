#include "Camera.h"
#include "Fire.h"
#include "Shader.h"
#include "Texture.h"
#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/matrix.hpp>
#include <iostream>
#include <random>
#include <stb_image.h>

glm::vec3 Fire::s_Accel = glm::vec3(0.0f, 0.2f, 0.0f);
unsigned int Fire::s_MaxParticles = 1000;

Fire::Fire()
	: m_Particles(), m_InstanceVBO(0), m_VBO(0), m_VAO(0), m_EBO(0),
	m_Texture("res/textures/circle.png", { GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR })
{
	std::random_device rd;
	m_Gen = std::mt19937(rd());

	std::vector<float> instanceAttributes;

	Particle particle = GenerateFireParticle();
	m_Particles.push_back(particle);

	// position
	instanceAttributes.push_back(particle.GetPosition().x);
	instanceAttributes.push_back(particle.GetPosition().y);
	instanceAttributes.push_back(particle.GetPosition().z);
	// color
	instanceAttributes.push_back(1.0f);
	instanceAttributes.push_back(0.0f);
	instanceAttributes.push_back(0.0f);

	std::vector<float> vertices;
	for (unsigned int i = 0; i < 4; i++)
	{
		vertices.push_back(m_Positions[i].x);
		vertices.push_back(m_Positions[i].y);
		vertices.push_back(m_Positions[i].z);
		vertices.push_back(m_TextureCoords[i].x);
		vertices.push_back(m_TextureCoords[i].y);
	}

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
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);

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
}

void Fire::Update(float dt)
{
	std::uniform_int_distribution<int> dis(-99, 99);
	std::uniform_int_distribution<int> disPositive(0, 99);

	if (m_Particles.size() < s_MaxParticles)
	{
		m_Particles.push_back(GenerateFireParticle());
	}

	unsigned int total = 0;
	for (const auto& particle : m_Particles)
	{
		if (particle.IsAlive()) total++;
	}

	if (total < s_MaxParticles)
	{
		for (unsigned int i = 0; i < m_Particles.size(); i++)
		{
			if (!m_Particles[i].IsAlive())
			{
				m_Particles[i] = GenerateFireParticle();
			}
		}
	}
	for (auto& particle : m_Particles)
	{
		if (particle.IsAlive())
		{
			particle.AddSpeed(s_Accel * dt);
			particle.Update(dt);
		}
	}
}

void Fire::Draw() const
{
	Camera& camera = Camera::GetInstance();

	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glDepthMask(GL_FALSE);

	m_Texture.Bind();

	Shader::FIRE->Bind();
	Shader::FIRE->SetInt("u_Texture", 0);
	glm::mat4 model = glm::mat4(1.0f);
	model[3][1] = 10.0f;

	Shader::FIRE->SetMat4f("u_Model", model);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

	std::vector<glm::vec3> updatedPositions = GetUpdatedPosition(camera);

	for (unsigned int i = 0; i < 4; i++)
	{
		glBufferSubData(GL_ARRAY_BUFFER, 5 * i * sizeof(float), 3 * sizeof(float), glm::value_ptr(updatedPositions[i]));
	}

	glBindBuffer(GL_ARRAY_BUFFER, m_InstanceVBO);
	for (unsigned int i = 0; i < m_Particles.size(); i++)
	{
		if (!m_Particles[i].IsAlive()) continue;
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

Particle Fire::GenerateFireParticle()
{
	std::uniform_int_distribution<int> dis(-10, 10);
	std::uniform_int_distribution<int> disPositive(0, 10);
	return Particle(
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(
			dis(m_Gen) / 100.0f,
			disPositive(m_Gen) / 100.0f,
			dis(m_Gen) / 100.0f
		)
	);
}

std::vector<glm::vec3> Fire::GetUpdatedPosition(const Camera& camera) const
{
	std::vector<glm::vec3> positions;

	// Primitive의 정면이 카메라를 향하도록 회전시킵니다.
	glm::vec3 z = glm::normalize(-camera.GetFront());
	glm::vec3 y = glm::normalize(camera.GetUp());
	glm::vec3 x = glm::normalize(glm::cross(z, y));

	glm::mat3 rot = glm::transpose(glm::mat3(x, y, z));

	for (unsigned int i = 0; i < 4; i++)
	{
		positions.push_back(rot * m_Positions[i]);
	}
	return positions;
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
