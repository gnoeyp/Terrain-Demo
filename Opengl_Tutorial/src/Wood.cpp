#include "Wood.h"

Wood::Wood(const glm::mat4& modelMatrix)
	: m_Model("res/textures/low_obj_1500/low_obj_1500.obj"), m_ModelMatrix(modelMatrix)
{
}

void Wood::Draw() const
{
	Shader::HOUSE->Bind();
	Shader::HOUSE->SetMat4f("u_Model", m_ModelMatrix);
	m_Model.Draw(*Shader::HOUSE);
}

void Wood::SetModelMatrix(const glm::mat4& modelMatrix)
{
	m_ModelMatrix = modelMatrix;
}
