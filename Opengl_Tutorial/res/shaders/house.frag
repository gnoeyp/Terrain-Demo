#version 410 core

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
in vec4 FragPosLightSpace;

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

uniform sampler2D u_ShadowMap;
uniform int u_EnableShadow;

uniform vec3 u_ViewPos;

float CalcShadow(vec4 fragPosLightSpace)
{
	if (u_EnableShadow == 0) return 0.0;
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    if (projCoords.z >= 1.0) return 0.0;
    float closestDepth = texture(u_ShadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;

    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(u_ShadowMap, 0);
	float bias = 0.001;
    for (int x = -1; x <= 1; ++x)
    {
        for (int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(u_ShadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }

    shadow /= 9.0;
	shadow /= 3.0;
    return shadow;
}

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

	float shadow = CalcShadow(FragPosLightSpace);

	FragColor = vec4(ambientColor + (1.0 - shadow) * (diffuseColor + specularColor + emissionColor), 1.0);

	BrightColor = vec4(emissionColor, 1.0);
}
