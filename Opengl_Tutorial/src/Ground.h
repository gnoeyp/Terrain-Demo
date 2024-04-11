#pragma once

#include "Shader.h"

class Ground
{
private:
	unsigned int m_VAO, m_VBO, m_EBO;
public:
	Ground();
	~Ground();
	void Draw(const Shader& shader) const;
};
