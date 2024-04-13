#include "GLUtils.h"
#include "UniformBlock.h"
#include <GL/glew.h>


unsigned int UniformBlock::s_BindingIndex = 0;

UniformBlock::UniformBlock(const std::vector<unsigned int>& sizes, const std::string& name)
	: m_Sizes(sizes), m_BindingIndex(s_BindingIndex), m_Name(name)
{
	glGenBuffers(1, &m_ID);
	glBindBuffer(GL_UNIFORM_BUFFER, m_ID);

	unsigned int total = 0;
	for (unsigned int size : sizes)
		total += size;

	glBufferData(GL_UNIFORM_BUFFER, total, NULL, GL_STATIC_DRAW);

	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glBindBufferRange(GL_UNIFORM_BUFFER, m_BindingIndex, m_ID, 0, total);
	++s_BindingIndex;
}

void UniformBlock::SetData(unsigned int index, const void* data)
{
	ASSERT(index < m_Sizes.size());
	glBindBuffer(GL_UNIFORM_BUFFER, m_ID);
	glBufferSubData(GL_UNIFORM_BUFFER, CalOffset(index), m_Sizes[index], data);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void UniformBlock::BindShader(const Shader& shader) const
{
	unsigned int index = glGetUniformBlockIndex(shader.GetID(), m_Name.c_str());
	glUniformBlockBinding(shader.GetID(), index, m_BindingIndex);
}

void UniformBlock::BindShader(const Shader* shaderPtr) const
{
	unsigned int index = glGetUniformBlockIndex(shaderPtr->GetID(), m_Name.c_str());
	glUniformBlockBinding(shaderPtr->GetID(), index, m_BindingIndex);
}

unsigned int UniformBlock::CalOffset(unsigned int index) const
{
	unsigned int result = 0;
	for (unsigned int i = 0; i < index; i++)
		result += m_Sizes[i];
	return result;
}
