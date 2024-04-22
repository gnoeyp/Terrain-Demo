#version 410 core

out vec4 color;

in VS_OUT
{
	vec3 FragPos;
	vec3 Normal;
	vec2 TexCoords;
} fs_in;

struct DirLight
{
	vec3 lightColor;
	vec3 lightDirection;
	float ambient;
	float shininess;
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
uniform sampler2D texture_emission1;

uniform vec3 u_ViewPos;
//uniform DirLight dirLight;

void main()
{
	vec3 ambientColor = dirLight.ambient * dirLight.lightColor;

	vec3 norm = normalize(fs_in.Normal);
	vec3 lightDir = normalize(-dirLight.lightDirection);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuseColor = dirLight.lightColor * diff * vec3(texture(texture_diffuse1, fs_in.TexCoords));

	vec3 viewDir = normalize(u_ViewPos - fs_in.FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0f);
	vec3 specularColor = dirLight.shininess * spec * vec3(texture(texture_specular1, fs_in.TexCoords));

	vec3 emissionColor = texture(texture_emission1, fs_in.TexCoords).rgb;

	color = vec4(ambientColor + diffuseColor + specularColor + emissionColor, 1.0);
}
