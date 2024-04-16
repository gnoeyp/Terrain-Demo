#version 410 core

in vec4 fColor;
in vec2 TexCoord;

out vec4 color;

uniform sampler2D u_Texture;

void main()
{
//	color = vec4(fColor, 0.2);
	color = fColor * vec4(1.0 - texture(u_Texture, TexCoord).rgb, texture(u_Texture, TexCoord).a);
}