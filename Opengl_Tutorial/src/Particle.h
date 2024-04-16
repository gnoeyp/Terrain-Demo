#pragma once

#include <glm/matrix.hpp>

class Particle
{
private:
	glm::vec3 m_Speed, m_Position;
	float m_Life;
	float m_MaxLife = 2.0;
public:
	Particle(glm::vec3 position, glm::vec3 speed);
	void Update(float dt);
	void AddSpeed(glm::vec3 accel);
	void Reset(const glm::vec3& position, const glm::vec3& speed);
	inline glm::vec3 GetPosition() const { return m_Position; }
	inline bool IsAlive() const { return m_Life < m_MaxLife; }
	inline float GetLife() const { return m_Life / m_MaxLife; }
};
