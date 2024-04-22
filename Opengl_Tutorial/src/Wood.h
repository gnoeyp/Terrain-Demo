#pragma once

#include "Model.h"
#include <glm/matrix.hpp>

class Wood
{
private:
	Model m_Models[2] = { Model("res/textures/low_obj_1500/low_obj_1500.obj"), Model("res/textures/low_obj_1500/low_obj_1500.obj") };
	glm::mat4 m_ModelMatrices[2];
	float m_OffsetX = 0.0f, m_OffsetY = 0.0f, m_OffsetZ = 0.0f;
public:
	Wood();
	void Draw() const;
	void ImGuiRender();
};
