#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

Camera::Camera(float x, float y, float z)
	: m_Position(x, y, z), m_Front(0, 0, -1.0f), m_WorldUp(0, 1.0, 0), m_Up()
{
}

Camera::~Camera()
{}

glm::mat4 Camera::GetViewMatrix() const
{
	return glm::lookAt(
		m_Position,
		m_Position + m_Front,
		m_WorldUp
	);
}

void Camera::MoveLeft(float dt)
{
	glm::vec3 right = glm::normalize(glm::cross(m_Front, m_WorldUp));
	m_Position -= m_Speed * dt * right;
}

void Camera::UpdateCameraVectors()
{
	m_Front = glm::normalize(glm::vec3(
		cos(glm::radians(m_Pitch)) * cos(glm::radians(m_Yaw)),
		sin(glm::radians(m_Pitch)),
		cos(glm::radians(m_Pitch)) * sin(glm::radians(m_Yaw))
	));
	glm::vec3 right = glm::normalize(glm::cross(m_Front, m_WorldUp));
	m_Up = glm::normalize(glm::cross(right, m_Front));
}

void Camera::Rotate(float dYaw, float dPitch)
{
	m_Yaw += dYaw;
	m_Pitch += dPitch;

	m_Pitch = std::max(m_Pitch, -89.f);
	m_Pitch = std::min(m_Pitch, 89.f);

	UpdateCameraVectors();
}

void Camera::MoveRight(float dt)
{
	glm::vec3 right = glm::normalize(glm::cross(m_Front, m_WorldUp));
	m_Position += m_Speed * dt * right;
}

void Camera::MoveFront(float dt)
{
	m_Position += m_Speed * dt * m_Front;
}

void Camera::MoveBack(float dt)
{
	m_Position -= m_Speed * dt * m_Front;
}

