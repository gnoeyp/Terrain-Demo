#version 330 core

layout(location = 0) in vec3 aPos;
//layout(location = 1) in vec3 aNormal;
layout (location = 1) in vec2 aTexCoord;

out float Height;
out vec2 TexCoord;
out vec3 FragPos;
//out vec3 Normal;

uniform mat4 u_Model = mat4(1.0);
uniform mat4 u_Proj = mat4(1.0);
uniform mat4 u_View = mat4(1.0);

void main()
{
	gl_Position = u_Proj * u_View * u_Model * vec4(aPos,  1.0);
	Height = aPos.y;
	TexCoord = aTexCoord;
	FragPos = vec3(u_Model * vec4(aPos, 1.0));
}