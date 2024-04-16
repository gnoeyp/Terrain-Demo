#include "Camera.h"
#include "Fire.h"
#include "GLUtils.h"
#include "Shader.h"
#include "Texture.h"
#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/matrix.hpp>
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <iostream>
#include <random>
#include <stb_image.h>

Fire::Fire(float x, float y, float z)
	: m_Particles(), m_InstanceVBO(0), m_VBO(0), m_VAO(0), m_EBO(0), m_Position(x, y, z),
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
	instanceAttributes.push_back(1.0f);

	std::vector<float> vertices;
	for (unsigned int i = 0; i < 4; i++)
	{
		vertices.push_back(m_VertexPositions[i].x);
		vertices.push_back(m_VertexPositions[i].y);
		vertices.push_back(m_VertexPositions[i].z);
		vertices.push_back(m_TextureCoords[i].x);
		vertices.push_back(m_TextureCoords[i].y);
	}

	unsigned int indices[] = {
		0, 1, 2,
		1, 3, 2
	};

	GLCall(glGenBuffers(1, &m_InstanceVBO));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_InstanceVBO));
	GLCall(glBufferData(GL_ARRAY_BUFFER, 7 * m_MaxParticles * sizeof(float), &instanceAttributes[0], GL_STATIC_DRAW));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));

	GLCall(glGenVertexArrays(1, &m_VAO));
	GLCall(glGenBuffers(1, &m_VBO));
	GLCall(glGenBuffers(1, &m_EBO));
	GLCall(glBindVertexArray(m_VAO));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_VBO));
	GLCall(glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW));

	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO));
	GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW));

	GLCall(glEnableVertexAttribArray(0));
	GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0));
	GLCall(glEnableVertexAttribArray(1));
	GLCall(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float))));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_InstanceVBO));
	GLCall(glEnableVertexAttribArray(2));
	GLCall(glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0));
	GLCall(glEnableVertexAttribArray(3));
	GLCall(glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float))));
	GLCall(glVertexAttribDivisor(2, 1));
	GLCall(glVertexAttribDivisor(3, 1));
}

void Fire::Update(float dt)
{
	std::uniform_int_distribution<int> dis(-99, 99);
	std::uniform_int_distribution<int> disPositive(0, 99);


	unsigned int total = 0;
	for (const auto& particle : m_Particles)
	{
		if (particle.IsAlive()) total++;
	}

	unsigned int numGenerated = 0;

	while (m_Particles.size() < m_MaxParticles && numGenerated++ < m_ParticlesPerFrame)
	{
		m_Particles.push_back(GenerateFireParticle());
	}

	for (unsigned int i = 0; i < m_Particles.size(); i++)
	{
		if (numGenerated >= m_ParticlesPerFrame)
			break;
		if (!m_Particles[i].IsAlive())
		{
			m_Particles[i] = GenerateFireParticle();
			numGenerated++;
		}
	}

	for (auto& particle : m_Particles)
	{
		if (particle.IsAlive())
		{
			particle.AddSpeed(glm::vec3(0.0f, m_Force, 0.0f));
			particle.Update(dt);
		}
	}
}

void Fire::ImGuiRender()
{
	if (ImGui::CollapsingHeader("Fire"))
	{
		ImGui::SliderFloat("Force", &m_Force, 0.0f, 0.1f);
		ImGui::SliderInt("Number of generated particles", &m_ParticlesPerFrame, 0, 10);
	}
}

void Fire::Draw() const
{
	Camera& camera = Camera::GetInstance();

	GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE));
	GLCall(glDepthMask(GL_FALSE));

	m_Texture.Bind();

	Shader::FIRE->Bind();
	Shader::FIRE->SetInt("u_Texture", 0);
	glm::mat4 model = glm::mat4(1.0f);
	model[3][0] = m_Position.x;
	model[3][1] = m_Position.y;
	model[3][2] = m_Position.z;

	Shader::FIRE->SetMat4f("u_Model", model);

	GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_VBO));

	std::vector<glm::vec3> updatedPositions = GetUpdatedPosition(camera);

	for (unsigned int i = 0; i < 4; i++)
	{
		GLCall(glBufferSubData(GL_ARRAY_BUFFER, 5 * i * sizeof(float), 3 * sizeof(float), glm::value_ptr(updatedPositions[i])));
	}

	GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_InstanceVBO));
	for (unsigned int i = 0; i < m_Particles.size(); i++)
	{
		if (!m_Particles[i].IsAlive()) {
			glm::vec4 color(0.0f, 0.0f, 0.0f, 0.0f);
			GLCall(glBufferSubData(GL_ARRAY_BUFFER, (i * 7 + 3) * sizeof(float), 4 * sizeof(float), &color[0]));
			continue;
		}
		glm::vec3 data = m_Particles[i].GetPosition();
		glm::vec4 color = glm::vec4(CalcColor(m_Particles[i].GetLife()), 1.0f);
		GLCall(glBufferSubData(GL_ARRAY_BUFFER, i * 7 * sizeof(float), 3 * sizeof(float), &data[0]));
		GLCall(glBufferSubData(GL_ARRAY_BUFFER, (i * 7 + 3) * sizeof(float), 4 * sizeof(float), &color[0]));
	}
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
	GLCall(glBindVertexArray(m_VAO));
	GLCall(glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, m_Particles.size()));
	GLCall(glBindVertexArray(0));

	m_Texture.Unbind();

	GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
	GLCall(glDepthMask(GL_TRUE));
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
		positions.push_back(rot * m_VertexPositions[i]);
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
