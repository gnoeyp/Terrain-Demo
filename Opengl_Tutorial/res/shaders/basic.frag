#version 330 core

out vec4 color;

in vec2 TexCoords;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_diffuse2;
uniform sampler2D texture_diffuse3;
uniform sampler2D texture_specular1;
uniform sampler2D texture_specular2;
uniform sampler2D texture_specular3;

uniform vec3 color_diffuse;
uniform vec3 color_specular;

void main()
{
	vec3 diffuseColor = vec3(texture(texture_diffuse1, TexCoords)) + color_diffuse;
	vec3 specularColor = vec3(texture(texture_specular1, TexCoords)) + color_specular;

	color = vec4(diffuseColor + specularColor, 1.0);
}
