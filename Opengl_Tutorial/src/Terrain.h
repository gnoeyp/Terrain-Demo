#pragma once

#include "Shader.h"
#include "Texture.h"
#include <glm/matrix.hpp>

enum class TerrainRenderMode { DEFAULT, WIREFRAME, NORMAL };
enum class TerrainRandomizationMode { NONE, ROTATE, VORONOI, OFFSET };

class Terrain
{
private:
	static const unsigned int REZ = 20;
	static const unsigned int NUM_PATCH_PTS = 4;
	unsigned int m_VAO, m_VBO, m_EBO, m_Normal;
	unsigned int m_NumStrips;
	unsigned int m_NumVertsPerStrip;
	TerrainRenderMode m_RenderMode = TerrainRenderMode::DEFAULT;
	TerrainRandomizationMode m_RandomizationMode = TerrainRandomizationMode::ROTATE;
	Texture m_Heightmap, m_Texture, m_Normalmap, m_MountainTexture, m_MountainNormalmap;
	glm::mat4 m_ModelMatrix;
public:
	Terrain(
		const char* heightmapPath,
		const char* texturePath,
		const char* normPath,
		const char* mountainTexturePath,
		const char* mountainNormalPath,
		const glm::mat4& modelMatrix
	);
	void Draw() const;
	void ImGuiRender();
};

