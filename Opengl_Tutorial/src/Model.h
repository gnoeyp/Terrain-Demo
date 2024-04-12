#pragma once

#include "Mesh.h"
#include "Shader.h"
#include <assimp/scene.h>
#include <string>
#include <vector>

class Model
{
public:
	Model(const std::string& path);
	void Draw(const Shader& shader) const;
	static unsigned int TextureFromFile(const char* path, const std::string& directory, bool gamma = false);
private:
	std::vector<std::unique_ptr<Mesh>> m_Meshes;
	std::vector<Texture> m_TexturesLoaded;
	std::string m_Directory;

	void LoadModel(const std::string& path);
	void ProcessNode(const aiNode* node, const aiScene* scene);
	std::unique_ptr<Mesh> ProcessMesh(const aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> LoadMaterialTextures(
		const aiMaterial* mat, aiTextureType type, const std::string& typeName);
};

