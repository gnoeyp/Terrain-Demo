#version 410 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aOffset;
layout (location = 3) in vec4 aColor;

layout (std140) uniform u_Matrices
{
    mat4 projection;
    mat4 view;
};

uniform mat4 u_Model;
uniform vec3 u_CameraFront;
uniform vec3 u_CameraUp;

out VS_OUT
{
	vec4 Color;
	vec2 TexCoord;
} vs_out;

void main()
{
	vec3 z = normalize(-u_CameraFront);
	vec3 y = normalize(u_CameraUp);
	vec3 x = normalize(cross(z, y));

	mat3 rot = transpose(mat3(x, y, z));

	gl_Position = projection * view * u_Model * vec4(aPos + aOffset, 1.0);
	vs_out.Color = aColor;
	vs_out.TexCoord = aTexCoord;
}