#version 330 core

struct DirLight
{
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

out vec4 color;

in float Height;
in vec2 TexCoord;
in vec3 FragPos;

uniform sampler2D u_Texture;
uniform DirLight dirLight;
uniform vec3 u_ViewPos;

void main()
{
	float ambientStrength = 0.1f;
	vec3 ambientColor = ambientStrength * dirLight.ambient;

	vec4 texColor = texture(u_Texture, TexCoord);

	float fogStart = 10.0f;
	float fogEnd = 500.0f;
	vec4 fogColor = vec4(0.59, 0.69, 0.85, 1.0f);
	float distance = length(FragPos - u_ViewPos);
	float visibility = 1.0;
	visibility = max(min((fogEnd - distance) / (fogEnd - fogStart), 1.0), 0.0);

	color = mix(fogColor, texColor, visibility);

	if (distance > 500.0)
		discard;
		
}
