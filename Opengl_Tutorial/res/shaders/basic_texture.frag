#version 330 core

out vec4 color;

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

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_diffuse2;
uniform sampler2D texture_diffuse3;
uniform sampler2D texture_specular1;
uniform sampler2D texture_specular2;
uniform sampler2D texture_specular3;

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

	color = vec4(ambientColor + diffuseColor + specularColor, 1.0);
}
