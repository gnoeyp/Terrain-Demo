#pragma once

#include "Model.h"
#include "Shader.h"
#include <glm/matrix.hpp>

class House
{
private:
	Model m_Model;
	glm::mat4 m_ModelMatrix;
public:
	House(const glm::mat4& modelMatrix);
	void Draw(const Shader& shader) const;
};
