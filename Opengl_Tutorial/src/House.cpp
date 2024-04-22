#include "Camera.h"
#include "House.h"
#include <glm/gtc/matrix_transform.hpp>
#include <imgui.h>

House::House()
	: m_Model("res/textures/middle-earth-house/AllHouse.obj"),
	m_ModelMatrix(
		glm::scale(
			glm::translate(
				glm::mat4(1.0f),
				glm::vec3(0.0f, 10.0f, 0.0f)),
			glm::vec3(1.0f, 1.0f, 1.0f)
		)
	)
{
}

void House::Draw(const Shader& shader) const
{
	glm::mat4 modelMatrix = m_ModelMatrix;
	modelMatrix[3][0] += m_OffsetX;
	modelMatrix[3][1] += m_OffsetY;
	modelMatrix[3][2] += m_OffsetZ;
	shader.Bind();
	shader.SetMat4f("u_Model", modelMatrix);
	shader.SetMat4f("u_NormalModel", glm::transpose(glm::inverse(modelMatrix)));
	shader.SetVec3f("u_ViewPos", Camera::GetInstance().GetPosition());
	m_Model.Draw(shader);
}

void House::ImGuiRender()
{
	if (ImGui::CollapsingHeader("House"))
	{
		ImGui::SliderFloat("House Offset X", &m_OffsetX, -20.0f, 20.0f);
		ImGui::SliderFloat("House Offset Y", &m_OffsetY, -20.0f, 20.0f);
		ImGui::SliderFloat("House Offset Z", &m_OffsetZ, -20.0f, 20.0f);
	}
}

