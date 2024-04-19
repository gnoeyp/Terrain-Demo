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
	m_ModelMatrix(modelMatrix)
{
	std::vector<float> positions;
	std::vector<float> heightmapCoords;
	std::vector<float> textureCoords;
	std::vector<float> normals, tangents, bitangents;
	float yScale = 64.0f / 256.0f, yShift = 16.0f;

	int width = m_Heightmap.GetWidth();
	int height = m_Heightmap.GetHeight();

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

			heightmapCoords.push_back(i / (float)REZ); // u
			heightmapCoords.push_back(j / (float)REZ); // v

			heightmapCoords.push_back((i + 1) / (float)REZ); // u
			heightmapCoords.push_back(j / (float)REZ); // v

			heightmapCoords.push_back(i / (float)REZ); // u
			heightmapCoords.push_back((j + 1) / (float)REZ); // v

			heightmapCoords.push_back((i + 1) / (float)REZ); // u
			heightmapCoords.push_back((j + 1) / (float)REZ); // v


			textureCoords.push_back(i * width / (float)REZ);
			textureCoords.push_back(j * height / (float)REZ);

			textureCoords.push_back((i + 1) * width / (float)REZ);
			textureCoords.push_back(j * height / (float)REZ);

			textureCoords.push_back(i * width / (float)REZ);
			textureCoords.push_back((j + 1) * height / (float)REZ);

			textureCoords.push_back((i + 1) * width / (float)REZ);
			textureCoords.push_back((j + 1) * height / (float)REZ);
		}
	}

	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);

	glGenBuffers(1, &m_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, (positions.size() + heightmapCoords.size() + textureCoords.size()) * sizeof(float), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, positions.size() * sizeof(float), &positions[0]);
	glBufferSubData(GL_ARRAY_BUFFER, positions.size() * sizeof(float), heightmapCoords.size() * sizeof(float), &heightmapCoords[0]);
	glBufferSubData(GL_ARRAY_BUFFER, (positions.size() + heightmapCoords.size()) * sizeof(float), textureCoords.size() * sizeof(float), &textureCoords[0]);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)(positions.size() * sizeof(float)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)((positions.size() + heightmapCoords.size()) * sizeof(float)));
	glEnableVertexAttribArray(2);

	glPatchParameteri(GL_PATCH_VERTICES, NUM_PATCH_PTS);
}

void Terrain::Draw() const
{
	if (m_Wireframe)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glBindVertexArray(m_VAO);

	m_Heightmap.Bind(0);
	m_Texture.Bind(1);
	m_Normalmap.Bind(2);
	m_MountainTexture.Bind(3);
	m_MountainNormalmap.Bind(4);

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
	Camera& camera = Camera::GetInstance();

	Shader::HEIGHTMAP->Bind();
	Shader::HEIGHTMAP->SetMat4f("u_Model", model);
	Shader::HEIGHTMAP->SetVec3f("u_ViewPos", camera.GetPosition());
	Shader::HEIGHTMAP->SetVec3f("u_FogColor", 0.35f, 0.46f, 0.56f);
	Shader::HEIGHTMAP->SetInt("u_Heightmap", 0);
	Shader::HEIGHTMAP->SetInt("u_Texture", 1);
	Shader::HEIGHTMAP->SetInt("u_NormalTexture", 2);
	Shader::HEIGHTMAP->SetInt("u_MountainTexture", 3);
	Shader::HEIGHTMAP->SetInt("u_MountainNormalTexture", 4);
	Shader::HEIGHTMAP->SetInt("u_TextureMethodType", m_RandomizaionMode);
	Shader::HEIGHTMAP->SetFloat("u_TexelSizeU", 1.0f / (float)m_Heightmap.GetWidth());
	Shader::HEIGHTMAP->SetFloat("u_TexelSizeV", 1.0f / (float)m_Heightmap.GetHeight());

	glDrawArrays(GL_PATCHES, 0, NUM_PATCH_PTS * REZ * REZ);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void Terrain::ImGuiRender()
{
	if (ImGui::CollapsingHeader("Terrain"))
	{
		ImGui::Checkbox("Wireframe", &m_Wireframe);

		ImGui::RadioButton("Terrain texture 1", &m_RandomizaionMode, 0);
		ImGui::SameLine();
		ImGui::RadioButton("Terrain texture 2", &m_RandomizaionMode, 1);
		ImGui::SameLine();
		ImGui::RadioButton("Terrain texture 3", &m_RandomizaionMode, 2);
	}
}

