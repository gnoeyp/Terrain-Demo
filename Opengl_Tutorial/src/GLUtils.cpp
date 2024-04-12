#include "GLUtils.h"
#include <iostream>

void GLClearError()
{
	while (glGetError() != GL_NO_ERROR) {}
}

bool GLLogError(const char* function, const char* path, int line)
{
	int error;
	if ((error = glGetError()) != GL_NO_ERROR)
	{
		std::cout << "[OpenGL Error (" << error << ")] '" << function << "', " << path << ": " << line << std::endl;
		return false;
	}
	return true;
}
