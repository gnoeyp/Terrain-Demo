#version 410 core

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

struct DirLight
{
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

layout (std140) uniform u_DirLight
{
	DirLight dirLight;
};

uniform sampler2D screenTexture;
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_diffuse2;
uniform sampler2D texture_diffuse3;
uniform sampler2D texture_specular1;
uniform sampler2D texture_specular2;
uniform sampler2D texture_specular3;
uniform sampler2D texture_emission1;

uniform vec3 u_ViewPos;
//uniform DirLight dirLight;

void main()
{
	float ambientStrength = 0.1f;
	vec3 ambientColor = ambientStrength * dirLight.ambient;

	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(-dirLight.direction);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuseColor = dirLight.diffuse * diff * vec3(texture(texture_diffuse1, TexCoords));

	vec3 viewDir = normalize(u_ViewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0f);
	vec3 specularColor = dirLight.specular * spec * vec3(texture(texture_specular1, TexCoords));

	vec3 emissionColor = texture(texture_emission1, TexCoords).rgb;

	FragColor = vec4(ambientColor + diffuseColor + specularColor + emissionColor, 1.0);

	float brightness = dot(FragColor.rgb, vec3(1.0, 0.80, 0.0));
	if (brightness > 1.0)
		BrightColor = vec4(FragColor.rgb, 1.0);
	else
		BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
}
