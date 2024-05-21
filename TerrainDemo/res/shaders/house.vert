#version 410 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

out VS_OUT
{
	vec3 FragPos;
	vec3 Normal;
	vec2 TexCoords;
	vec4 FragPosLightSpace;
} vs_out;

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
	vs_out.FragPos = vec3(u_Model * vec4(aPos, 1.0));
	vs_out.Normal = mat3(u_NormalModel) * aNormal;
	vs_out.TexCoords = aTexCoords;
	vs_out.FragPosLightSpace = lightSpaceMatrix * vec4(vs_out.FragPos, 1.0);
	gl_Position = projection * view * u_Model * vec4(aPos,  1.0);
}