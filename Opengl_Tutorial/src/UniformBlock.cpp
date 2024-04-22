#include "GLUtils.h"
#include "GLUtils.h"
#include "UniformBlock.h"
#include <GL/glew.h>


unsigned int UniformBlock::s_BindingIndex = 0;

UniformBlock::UniformBlock(
	const std::vector<unsigned int>& begin,
	const std::vector<unsigned int>& sizes,
	const std::string& name)
	: m_Begin(begin), m_Sizes(sizes), m_BindingIndex(s_BindingIndex), m_Name(name)
{
	ASSERT(m_Begin.size() == m_Sizes.size());

	glGenBuffers(1, &m_ID);
	glBindBuffer(GL_UNIFORM_BUFFER, m_ID);

	int total = m_Begin[m_Begin.size() - 1] + m_Sizes[m_Sizes.size() - 1];
	glBufferData(GL_UNIFORM_BUFFER, total, NULL, GL_STATIC_DRAW);

	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glBindBufferRange(GL_UNIFORM_BUFFER, m_BindingIndex, m_ID, 0, total);
	++s_BindingIndex;
}

void UniformBlock::SetData(unsigned int index, const void* data)
{
	ASSERT(index < m_Sizes.size());
	glBindBuffer(GL_UNIFORM_BUFFER, m_ID);
	glBufferSubData(GL_UNIFORM_BUFFER, m_Begin[index], m_Sizes[index], data);
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
