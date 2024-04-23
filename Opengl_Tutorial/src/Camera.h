#pragma once

#include "glm/matrix.hpp"
class Camera
{
private:
	Camera(float x = 0.0, float y = 0.0, float z = 0.0);

	glm::vec3 m_Position;
	glm::vec3 m_Front;
	glm::vec3 m_Up;
	glm::vec3 m_WorldUp;
	float m_Speed = 100.0f;
	float m_Yaw = -90.f;
	float m_Pitch = 0.0f;
	float m_Zoom = 45.0f;
public:
	~Camera();
	static Camera& GetInstance()
	{
		static Camera camera(0.0f, 12.0f, 8.0f);
		return camera;
	}

	glm::mat4 GetViewMatrix() const;
	void MoveLeft(float dt);
	void MoveRight(float dt);
	void MoveFront(float dt);
	void MoveBack(float dt);
	void Rotate(float dYaw, float dPitch);
	void UpdateCameraVectors();
	const glm::vec3& GetPosition() const { return m_Position; }
	inline float GetZoom() const { return m_Zoom; }
	inline glm::vec3 GetFront() const { return m_Front; }
	inline glm::vec3 GetUp() const { return m_Up; }
	inline void SetSpeed(float speed) { m_Speed = speed; }
	inline float GetSpeed() const { return m_Speed; }
};

