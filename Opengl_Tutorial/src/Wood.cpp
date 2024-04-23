#include "Wood.h"
#include <glm/gtc/matrix_transform.hpp>
#include <imgui.h>

Wood::Wood()
{
	m_ModelMatrices[0] = glm::scale(
		glm::translate(glm::mat4(1.0f), glm::vec3(3.0f, 0.46f, 5.0f)),
		glm::vec3(0.02f, 0.02f, 0.02f)
	);

	m_ModelMatrices[1] = glm::rotate(
		glm::rotate(
			glm::scale(
				glm::translate(glm::mat4(1.0f), glm::vec3(2.7f, 0.46f, 5.3f)),
				glm::vec3(0.02f, 0.02f, 0.02f)
			),
			glm::radians(90.f), glm::vec3(0.0f, 1.0f, 0.0f)
		),
		glm::radians(-15.f), glm::vec3(1.0f, 0.0f, 0.0f)
	);
}

void Wood::Draw() const
{
	for (unsigned int i = 0; i < 2; i++)
	{
		glm::mat4 modelMatrix = m_ModelMatrices[i];
		modelMatrix[3][0] += m_OffsetX;
		modelMatrix[3][1] += m_OffsetY;
		modelMatrix[3][2] += m_OffsetZ;
		Shader::HOUSE->Bind();
		Shader::HOUSE->SetMat4f("u_Model", modelMatrix);
		m_Models[i].Draw(*Shader::HOUSE);
	}
}

void Wood::ImGuiRender()
{
	if (ImGui::CollapsingHeader("Wood"))
	{
		ImGui::SliderFloat("OffsetX", &m_OffsetX, -20.0f, 20.0f);
		ImGui::SliderFloat("OffsetY", &m_OffsetY, -20.0f, 20.0f);
		ImGui::SliderFloat("OffsetZ", &m_OffsetZ, -20.0f, 20.0f);
	}
}

