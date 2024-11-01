#include "Particle.h"

Particle::Particle(glm::vec3 position, glm::vec3 speed) : m_Life(0), m_Speed(speed), m_Position(position)
{
}

void Particle::Update(float dt)
{
	if (!IsAlive()) return;
	m_Position += m_Speed * dt;
	// TODO: life 변경방법 수정
	if (m_Life < m_MaxLife)
		m_Life += dt;
}

void Particle::AddSpeed(glm::vec3 accel)
{
	m_Speed += accel;
}

void Particle::Reset(const glm::vec3& position, const glm::vec3& speed)
{
	m_Life = m_MaxLife;
	m_Position = position;
	m_Speed = speed;
}

