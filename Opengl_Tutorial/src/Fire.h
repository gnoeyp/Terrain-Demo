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
	unsigned int m_NumGen = 3;
public:
	Fire();
	void Update();
	void Draw(const Camera& camera) const;
	FireParticle GenerateFireParticle();
	glm::vec3 CalcColor(float life) const;
};