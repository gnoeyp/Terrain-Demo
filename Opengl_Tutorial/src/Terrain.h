#pragma once

#include "Shader.h"

class Terrain
{
private:
	unsigned int m_VAO, m_VBO, m_EBO, m_Tex;
	unsigned int m_NumStrips;
	unsigned int m_NumVertsPerStrip;
public:
	Terrain();
	void Draw() const;
};

