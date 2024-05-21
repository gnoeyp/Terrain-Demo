#version 410 core

in VS_OUT
{
	vec4 Color;
	vec2 TexCoord;
} fs_in;

out vec4 FragColor;

uniform sampler2D u_Texture;

void main()
{
	FragColor = fs_in.Color * vec4(1.0 - texture(u_Texture, fs_in.TexCoord).rgb, texture(u_Texture, fs_in.TexCoord).a);
}