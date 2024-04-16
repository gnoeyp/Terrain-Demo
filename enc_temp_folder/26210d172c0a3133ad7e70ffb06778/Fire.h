#pragma once

#include "Camera.h"
#include "FireParticle.h"
#include "Texture.h"
#include <random>
#include <vector>

class Fire
{
private:
	unsigned int m_InstanceVBO, m_VAO, m_VBO, m_EBO, m_Tex;
	Texture m_Texture;
	std::vector<FireParticle> m_Particles;
	std::mt19937 m_Gen;
	glm::vec3 m_Positions[4] = {
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
	FireParticle GenerateFireParticle();
	std::vector<glm::vec3> GetUpdatedPosition(const Camera& camera) const;
public:
	Fire();
	void Update();
	void Draw(const Camera& camera) const;
	glm::vec3 CalcColor(float life) const;
};