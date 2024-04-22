#pragma once

#include "Model.h"
#include <glm/matrix.hpp>

class Wood
{
private:
	Model m_Model;
	glm::mat4 m_ModelMatrix;
public:
	Wood(const glm::mat4& modelMatrix);
	void Draw() const;
	void SetModelMatrix(const glm::mat4& modelMatrix);
};
