#version 410 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;
out vec4 FragPosLightSpace;

layout (std140) uniform u_Matrices
{
    mat4 projection;
    mat4 view;
};

layout (std140) uniform u_LightSpaceMatrix
{
    mat4 lightSpaceMatrix;
};

uniform mat4 u_Model;
uniform mat4 u_NormalModel;

void main()
{
	FragPos = vec3(u_Model * vec4(aPos, 1.0));
	Normal = mat3(u_NormalModel) * aNormal;
	TexCoords = aTexCoords;
	gl_Position = projection * view * u_Model * vec4(aPos,  1.0);
	FragPosLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0);
}