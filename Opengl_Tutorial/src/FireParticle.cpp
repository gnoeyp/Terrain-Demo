#include "FireParticle.h"

FireParticle::FireParticle(glm::vec3 position, glm::vec3 speed) : m_Life(0), m_Speed(speed), m_Position(position)
{
}

void FireParticle::Update()
{
	if (IsDead()) return;
	m_Position += m_Speed;
	// TODO: life 변경방법 수정
	if (m_Life < m_MaxLife)
		m_Life++;
}

void FireParticle::AddSpeed(glm::vec3 accel)
{
	m_Speed += accel;
}

void FireParticle::Reset(const glm::vec3& position, const glm::vec3& speed)
{
	m_Life = m_MaxLife;
	m_Position = position;
	m_Speed = speed;
}

