#version 410 core

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D u_Scene;
uniform sampler2D u_BloomBlur;

void main()
{
	const float gamma = 1.2;
	const float exposure = 1.0;

	vec3 sceneColor = texture(u_Scene, TexCoords).rgb;

	vec3 bloomColor = texture(u_BloomBlur, TexCoords).rgb;

	vec3 result = vec3(1.0) - exp(-(sceneColor + bloomColor) * exposure);
	result = pow(result, vec3(1.0 / gamma));

	FragColor = vec4(result, 1.0);

//	FragColor = vec4(sceneColor + bloomColor, 1.0);
//	FragColor = vec4(sceneColor + bloomColor, 1.0);
}