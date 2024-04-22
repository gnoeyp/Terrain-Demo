#pragma once

#include "Model.h"
#include "Shader.h"
#include <glm/matrix.hpp>

class House
{
private:
	Model m_Model;
	glm::mat4 m_ModelMatrix;
	float m_OffsetX = 0.0f, m_OffsetY = 10.0f, m_OffsetZ = 0.0f;
public:
	House();
	void Draw(const Shader& shader) const;
	void ImGuiRender();
};
