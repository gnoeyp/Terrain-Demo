#include "Camera.h"
#include "House.h"
#include "Shader.h"
#include <glm/gtc/matrix_transform.hpp>

House::House(const glm::mat4& modelMatrix)
	: m_Model("res/textures/middle-earth-house/AllHouse.obj"), m_ModelMatrix(modelMatrix)
{
	m_ModelMatrix = glm::mat4(1.0f);
	m_ModelMatrix = glm::translate(m_ModelMatrix, glm::vec3(0.0f, 10.0f, 0.0f));
	m_ModelMatrix = glm::scale(m_ModelMatrix, glm::vec3(1.0f, 1.0f, 1.0f));
}

void House::Draw() const
{
	Shader::HOUSE->SetMat4f("u_Model", m_ModelMatrix);
	Shader::HOUSE->SetVec3f("u_ViewPos", Camera::GetInstance().GetPosition());
	m_Model.Draw(*Shader::HOUSE);
}
