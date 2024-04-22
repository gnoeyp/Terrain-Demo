#pragma once

#include "Shader.h"
#include <string>
#include <vector>

class UniformBlock
{
private:
	unsigned int m_ID, m_BindingIndex;
	std::vector<unsigned int> m_Sizes, m_Begin;
	std::string m_Name;
	UniformBlock& operator=(const UniformBlock&);
	UniformBlock(const UniformBlock&);
public:
	UniformBlock(
		const std::vector<unsigned int>& begin,
		const std::vector<unsigned int>& sizes,
		const std::string& name);
	void SetData(unsigned int index, const void* data);
	void BindShader(const Shader& shader) const;
	void BindShader(const Shader* shaderPtr) const;
	static unsigned int s_BindingIndex;
};
