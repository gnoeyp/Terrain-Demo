#pragma once

#include "Shader.h"
#include <string>
#include <vector>

class UniformBlock
{
private:
	unsigned int m_ID, m_BindingIndex;
	std::vector<unsigned int> m_Sizes;
	std::string m_Name;
	unsigned int CalOffset(unsigned int index) const;
	UniformBlock& operator=(const UniformBlock&);
	UniformBlock(const UniformBlock&);
public:
	UniformBlock(const std::vector<unsigned int>& sizes, const std::string& name);
	void SetData(unsigned int index, const void* data);
	void BindShader(const Shader& shader) const;
	void BindShader(const Shader* shaderPtr) const;
	static unsigned int s_BindingIndex;
};
