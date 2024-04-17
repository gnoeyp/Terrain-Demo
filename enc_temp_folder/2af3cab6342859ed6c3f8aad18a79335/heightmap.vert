#version 410 core

layout(location = 0) in vec3 aPos;
layout (location = 1) in vec2 aHeightmapCoord;
layout (location = 2) in vec2 aTexCoord;
//layout (location = 2) in vec3 aTangent;
//layout (location = 3) in vec3 aNormal;
//
layout (std140) uniform u_Matrices
{
    mat4 projection;
    mat4 view;
};

out float Height;
out vec2 TexCoord;
out vec2 HCoord;
//out vec3 FragPos;
//out vec3 TangentLightPos;
//out vec3 TangentViewPos;
//out vec3 TangentFragPos;
//
uniform mat4 u_Model = mat4(1.0);
uniform vec3 u_ViewPos;
uniform vec3 u_LightPos;

void main()
{
	gl_Position = vec4(aPos, 1.0);
//	gl_Position = projection * view * u_Model * vec4(aPos,  1.0);
//	Height = aPos.y;
	TexCoord = aTexCoord;
	HCoord = aHeightmapCoord;
//	FragPos = vec3(u_Model * vec4(aPos, 1.0));
//	vec3 T = normalize(vec3(u_Model * vec4(aTangent, 0.0)));
//	vec3 N = normalize(vec3(u_Model * vec4(aNormal, 0.0)));
//	vec3 B = normalize(cross(N, T));
//	mat3 TBN = transpose(mat3(T, B, N));
//
//	TangentLightPos = TBN * u_LightPos;
//	TangentViewPos = TBN * u_ViewPos;
//	TangentFragPos = TBN * FragPos;
}