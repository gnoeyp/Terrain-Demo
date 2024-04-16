#pragma once

#include "Particle.h"
#include "Texture.h"
#include <random>
#include <vector>

class Fire
{
private:
	unsigned int m_InstanceVBO, m_VAO, m_VBO, m_EBO, m_Tex;
	Texture m_Texture;
	std::vector<Particle> m_Particles;
	std::mt19937 m_Gen;
	glm::vec3 m_Position;
	glm::vec3 m_VertexPositions[4] = {
		glm::vec3(-0.05f,  -0.05f, 0.0f),
		glm::vec3(-0.05f, 0.05f, 0.0f),
		glm::vec3(0.05f, -0.05f, 0.0f),
		glm::vec3(0.05f, 0.05f, 0.0f)
	};
	glm::vec2 m_TextureCoords[4] = {
		glm::vec2(0.0f, 0.0f),
		glm::vec2(0.0f, 1.0f),
		glm::vec2(1.0f, 0.0f),
		glm::vec2(1.0f, 1.0f),
	};
	Particle GenerateFireParticle();
	std::vector<glm::vec3> GetUpdatedPosition(const Camera& camera) const;
	int m_ParticlesPerFrame = 3;
	const unsigned int m_MaxParticles = 1000;
	float m_Force = 0.015f;
public:
	Fire(float x, float y, float z);
	void Update(float dt);
	void ImGuiRender();
	void Draw() const;
	glm::vec3 CalcColor(float life) const;
};