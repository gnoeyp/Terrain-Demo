#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

out vec2 TexCoords;

uniform mat4 u_Model = mat4(1.0);
uniform mat4 u_Proj = mat4(1.0);
uniform mat4 u_View = mat4(1.0);

void main()
{
TexCoords = aTexCoords;
	gl_Position = u_Proj * u_View * u_Model * vec4(aPos,  1.0);
}