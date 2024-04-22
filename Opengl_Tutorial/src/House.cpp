#include "Camera.h"
#include "House.h"
#include <glm/gtc/matrix_transform.hpp>

House::House(const glm::mat4& modelMatrix)
	: m_Model("res/textures/middle-earth-house/AllHouse.obj"), m_ModelMatrix(modelMatrix)
{
}

void House::Draw(const Shader& shader) const
{
	shader.Bind();
	shader.SetMat4f("u_Model", m_ModelMatrix);
	shader.SetVec3f("u_ViewPos", Camera::GetInstance().GetPosition());
	m_Model.Draw(shader);
}
