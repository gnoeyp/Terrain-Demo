#version 410 core

layout(location = 0) in vec3 aPos;
layout (location = 1) in vec2 aHeightmapCoord;
layout (location = 2) in vec2 aTexCoord;

out VS_OUT
{
	vec2 TexCoord;
	vec2 HCoord;
	vec3 WorldPos;
} vs_out;

uniform mat4 u_Model = mat4(1.0);

void main()
{
	vs_out.TexCoord = aTexCoord;
	vs_out.HCoord = aHeightmapCoord;
	vs_out.WorldPos = aPos;
	gl_Position = vec4(aPos, 1.0);
}