#version 410 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;
 
layout (std140) uniform u_Matrices
{
    mat4 projection;
    mat4 view;
};

uniform mat4 u_Model;

void main()
{
	gl_Position = projection * view * u_Model * vec4(aPos, 1.0);
	TexCoord = aTexCoord;
}
