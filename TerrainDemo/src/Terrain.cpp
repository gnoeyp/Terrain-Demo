#include "Mesh.h"
#include "Terrain.h"
#include "Texture.h"
#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/matrix.hpp>
#include <imgui.h>
#include <iostream>
#include <stb_image.h>
#include <vector>

Terrain::Terrain(
	const char* heightmapPath,
	const char* texturePath,
	const char* normalPath,
	const char* mountainTexturePath,
	const char* mountainNormalPath,
	const glm::mat4& modelMatrix
)
	: m_Heightmap(heightmapPath, { GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR }),
	m_Texture(texturePath, { GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR }),
	m_Normalmap(normalPath, { GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR }),
	m_MountainTexture(mountainTexturePath, { GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR }),
	m_MountainNormalmap(mountainNormalPath, { GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR }),
	m_ModelMatrix(modelMatrix),
	m_GrassTexture("res/textures/terrainTexture.jpg", { GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR })
{
	std::vector<float> positions;
	std::vector<float> heightmapCoords;
	std::vector<float> textureCoords;
	std::vector<float> normals, tangents, bitangents;
	float yScale = 64.0f / 256.0f, yShift = 16.0f;

	int width = 2000;
	int height = 2000;

	for (unsigned int i = 0; i < REZ; i++)
	{
		for (unsigned int j = 0; j < REZ; j++)
		{
			positions.push_back(-width / 2.0f + width * i / (float)REZ); // v.x
			positions.push_back(0.0f); // v.y
			positions.push_back(-height / 2.0f + height * j / (float)REZ); // v.z

			positions.push_back(-width / 2.0f + width * (i + 1) / (float)REZ); // v.x
			positions.push_back(0.0f); // v.y
			positions.push_back(-height / 2.0f + height * j / (float)REZ); // v.z

			positions.push_back(-width / 2.0f + width * i / (float)REZ); // v.x
			positions.push_back(0.0f); // v.y
			positions.push_back(-height / 2.0f + height * (j + 1) / (float)REZ); // v.z

			positions.push_back(-width / 2.0f + width * (i + 1) / (float)REZ); // v.x
			positions.push_back(0.0f); // v.y
			positions.push_back(-height / 2.0f + height * (j + 1) / (float)REZ); // v.z

			textureCoords.push_back(i / (float)REZ);
			textureCoords.push_back(j / (float)REZ);

			textureCoords.push_back((i + 1) / (float)REZ);
			textureCoords.push_back(j / (float)REZ);

			textureCoords.push_back(i / (float)REZ);
			textureCoords.push_back((j + 1) / (float)REZ);

			textureCoords.push_back((i + 1) / (float)REZ);
			textureCoords.push_back((j + 1) / (float)REZ);
		}
	}


	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);

	glGenBuffers(1, &m_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, (positions.size() + textureCoords.size()) * sizeof(float), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, positions.size() * sizeof(float), &positions[0]);
	glBufferSubData(GL_ARRAY_BUFFER, positions.size() * sizeof(float), textureCoords.size() * sizeof(float), &textureCoords[0]);
	//glBufferSubData(GL_ARRAY_BUFFER, (positions.size() + heightmapCoords.size()) * sizeof(float), textureCoords.size() * sizeof(float), &textureCoords[0]);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)(positions.size() * sizeof(float)));
	//glEnableVertexAttribArray(1);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)((positions.size()) * sizeof(float)));
	glEnableVertexAttribArray(1);

	glPatchParameteri(GL_PATCH_VERTICES, NUM_PATCH_PTS);
}

void Terrain::Draw() const
{
	if (m_RenderMode == TerrainRenderMode::WIREFRAME)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	Shader& shader = m_RenderMode == TerrainRenderMode::NORMAL ? *Shader::HEIGHTMAP_NORMAL : *Shader::HEIGHTMAP;

	glBindVertexArray(m_VAO);

	m_Heightmap.Bind(0);
	m_Texture.Bind(1);
	m_Normalmap.Bind(2);
	m_MountainTexture.Bind(3);
	m_MountainNormalmap.Bind(4);
	m_GrassTexture.Bind(5);

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
	Camera& camera = Camera::GetInstance();

	shader.Bind();
	shader.SetMat4f("u_Model", model);
	shader.SetVec3f("u_ViewPos", camera.GetPosition());
	shader.SetVec3f("u_FogColor", 0.35f, 0.46f, 0.56f);
	shader.SetInt("u_Heightmap", 0);
	shader.SetInt("u_Texture", 1);
	shader.SetInt("u_NormalTexture", 2);
	shader.SetInt("u_MountainTexture", 3);
	shader.SetInt("u_MountainNormalTexture", 4);
	shader.SetInt("u_Grass2Texture", 5);


	shader.SetFloat("u_GDispFactor", m_GDispFactor);
	shader.SetFloat("u_Power", m_Power);
	shader.SetFloat("u_Freq", m_Freq);
	shader.SetInt("u_Octaves", m_Octaves);
	shader.SetFloat("u_FogStart", m_FogStart);
	shader.SetFloat("u_FogEnd", m_FogEnd);

	int textureMethodType = 0;
	switch (m_RandomizationMode)
	{
	case TerrainRandomizationMode::NONE:
		textureMethodType = 0;
		break;
	case TerrainRandomizationMode::ROTATE:
		textureMethodType = 1;
		break;
	case TerrainRandomizationMode::VORONOI:
		textureMethodType = 2;
		break;
	case TerrainRandomizationMode::OFFSET:
		textureMethodType = 3;
		break;
	}
	shader.SetInt("u_TextureMethodType", textureMethodType);
	shader.SetFloat("u_TexelSizeU", 1.0f / (float)m_Heightmap.GetWidth());
	shader.SetFloat("u_TexelSizeV", 1.0f / (float)m_Heightmap.GetHeight());

	glDrawArrays(GL_PATCHES, 0, NUM_PATCH_PTS * REZ * REZ);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void Terrain::ImGuiRender()
{
	if (ImGui::CollapsingHeader("Terrain"))
	{
		if (ImGui::RadioButton("Default", m_RenderMode == TerrainRenderMode::DEFAULT))
			m_RenderMode = TerrainRenderMode::DEFAULT;
		ImGui::SameLine();
		if (ImGui::RadioButton("Wireframe", m_RenderMode == TerrainRenderMode::WIREFRAME))
			m_RenderMode = TerrainRenderMode::WIREFRAME;
		ImGui::SameLine();
		if (ImGui::RadioButton("Normal", m_RenderMode == TerrainRenderMode::NORMAL))
			m_RenderMode = TerrainRenderMode::NORMAL;


		if (ImGui::RadioButton("NONE", m_RandomizationMode == TerrainRandomizationMode::NONE))
			m_RandomizationMode = TerrainRandomizationMode::NONE;
		ImGui::SameLine();
		if (ImGui::RadioButton("Rotate", m_RandomizationMode == TerrainRandomizationMode::ROTATE))
			m_RandomizationMode = TerrainRandomizationMode::ROTATE;
		ImGui::SameLine();
		if (ImGui::RadioButton("Voronoi", m_RandomizationMode == TerrainRandomizationMode::VORONOI))
			m_RandomizationMode = TerrainRandomizationMode::VORONOI;
		ImGui::SameLine();
		if (ImGui::RadioButton("Offset", m_RandomizationMode == TerrainRandomizationMode::OFFSET))
			m_RandomizationMode = TerrainRandomizationMode::OFFSET;

		ImGui::SliderFloat("Displacement Factor", &m_GDispFactor, 0, 30);
		ImGui::SliderFloat("Power", &m_Power, 0, 10.0);
		ImGui::SliderFloat("Frequency", &m_Freq, 0, 1.0);
		int octave = (int)m_Octaves;
		if (ImGui::SliderInt("Octaves", &octave, 0, 30))
			m_Octaves = octave;

		ImGui::DragFloatRange2("Fog", &m_FogStart, &m_FogEnd, 1.0f, 0.0f, 5000.0f);
	}
}

