#version 410 core

layout(location = 0) in vec3 aPos;
layout (location = 1) in vec2 aHeightmapCoord;
layout (location = 2) in vec2 aTexCoord;

out float Height;
out vec2 TexCoord;
out vec2 HCoord;

uniform mat4 u_Model = mat4(1.0);

void main()
{
	gl_Position = vec4(aPos, 1.0);
	TexCoord = aTexCoord;
	HCoord = aHeightmapCoord;
}