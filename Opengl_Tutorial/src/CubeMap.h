#pragma once

#include "Shader.h"
#include <string>
#include <vector>

class CubeMap
{
private:
	unsigned int m_ID, m_VAO, m_VBO;
public:
	CubeMap(std::vector<std::string> faces);
	void Draw(const Shader&) const;
};

