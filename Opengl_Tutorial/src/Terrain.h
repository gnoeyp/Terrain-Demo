#pragma once

#include "Shader.h"
#include "Texture.h"

class Terrain
{
private:
	unsigned int m_VAO, m_VBO, m_EBO, m_Normal;
	unsigned int m_NumStrips;
	unsigned int m_NumVertsPerStrip;
	Texture m_Heightmap, m_Texture, m_Normalmap;
public:
	Terrain(const char* heightmapPath, const char* texturePath, const char* normPath);
	void Draw(const Shader& shader) const;
};

