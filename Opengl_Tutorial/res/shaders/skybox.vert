#version 410 core
layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

uniform mat4 u_Proj;
uniform mat4 u_View;

void main()
{
	vec4 pos = u_Proj * u_View * vec4(aPos, 1.0);
	gl_Position = vec4(pos.xyww);
	TexCoords = aPos;
}
