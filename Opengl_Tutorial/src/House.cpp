#include "Camera.h"
#include "House.h"
#include <glm/gtc/matrix_transform.hpp>
#include <imgui.h>

House::House()
	: m_Model("res/textures/middle-earth-house/AllHouse.obj"),
	m_OffsetX(0.0f),
	m_OffsetY(10.0f),
	m_OffsetZ(0.0f),
	m_ModelMatrix()
{
	m_ModelMatrix = glm::scale(
		glm::translate(
			glm::mat4(1.0f),
			glm::vec3(m_OffsetX, m_OffsetY, m_OffsetZ)),
		glm::vec3(1.0f, 1.0f, 1.0f)
	);
}

void House::Draw(const Shader& shader) const
{
	shader.Bind();
	shader.SetMat4f("u_Model", m_ModelMatrix);
	shader.SetVec3f("u_ViewPos", Camera::GetInstance().GetPosition());
	m_Model.Draw(shader);
}

void House::ImGuiRender()
{
	if (ImGui::CollapsingHeader("House"))
	{
		if (ImGui::SliderFloat("Offset X", &m_OffsetX, -20.0f, 20.0f))
			m_ModelMatrix[3][0] = m_OffsetX;
		if (ImGui::SliderFloat("Offset Y", &m_OffsetY, -20.0f, 20.0f))
			m_ModelMatrix[3][1] = m_OffsetY;
		if (ImGui::SliderFloat("Offset Z", &m_OffsetZ, -20.0f, 20.0f))
			m_ModelMatrix[3][2] = m_OffsetZ;
	}
}

