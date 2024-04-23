#version 410 core
layout(quads, fractional_odd_spacing, ccw) in;

uniform sampler2D u_HeightMap;

uniform float u_GDispFactor;
uniform float u_Power;
uniform float u_Freq;
uniform int u_Octaves;

layout (std140) uniform u_Matrices
{
    mat4 projection;
    mat4 view;
};

layout (std140) uniform u_LightSpaceMatrix
{
    mat4 lightSpaceMatrix;
};

uniform mat4 u_Model = mat4(1.0);
uniform float u_TexelSizeU;
uniform float u_TexelSizeV;

in TC_OUT
{
	vec2 TexCoord;
} te_in[];

out TE_OUT
{
	vec3 FragPos;
    vec2 TexCoord;
	vec2 HeightCoord;
	vec4 FragPosLightSpace;
} te_out;

vec3 seed = vec3(0.0, 0.1, 0.3);

float Random2D(in vec2 st)
{
	return fract(sin(dot(st.xy, vec2(12.9898, 78.233) + seed.xy)) * 43758.5453123);
}


float InterpolatedNoise(int ind, float x, float y) {
	int integer_X = int(floor(x));
	float fractional_X = fract(x);
	int integer_Y = int(floor(y));
	float fractional_Y = fract(y);
	vec2 randomInput = vec2(integer_X, integer_Y);
	float a = Random2D(randomInput);
	float b = Random2D(randomInput + vec2(1.0, 0.0));
	float c = Random2D(randomInput + vec2(0.0, 1.0));
	float d = Random2D(randomInput + vec2(1.0, 1.0));

	vec2 w = vec2(fractional_X, fractional_Y);
	w = w*w*w*(10.0 + w*(-15.0 + 6.0*w));

	float k0 = a, 
	k1 = b - a, 
	k2 = c - a, 
	k3 = d - c - b + a;

	return k0 + k1*w.x + k2*w.y + k3*w.x*w.y;
}

float perlin(vec2 st){
    const mat2 m = mat2(0.8,-0.6,0.6,0.8);
    
	float persistence = 0.5;
	float total = 0.0,
		frequency = 0.05*u_Freq,
		amplitude = u_GDispFactor;
	for (int i = 0; i < u_Octaves; ++i) {
		frequency *= 2.0;
		amplitude *= persistence;

		vec2 v = frequency*m*(st + 10.0f);

		total += InterpolatedNoise(0, v.x, v.y) * amplitude;
	}

	return pow(total, u_Power);
}

void main()
{
    float u = gl_TessCoord.x;
    float v = gl_TessCoord.y;

    vec2 t00 = te_in[0].TexCoord;
    vec2 t01 = te_in[1].TexCoord;
    vec2 t10 = te_in[2].TexCoord;
    vec2 t11 = te_in[3].TexCoord;

    vec2 t0 = (t01 - t00) * u + t00;
    vec2 t1 = (t11 - t10) * u + t10;
    te_out.TexCoord = (t1 - t0) * v + t0;

    vec4 p00 = gl_in[0].gl_Position;
    vec4 p01 = gl_in[1].gl_Position;
    vec4 p10 = gl_in[2].gl_Position;
    vec4 p11 = gl_in[3].gl_Position;

    vec4 uVec = p01 - p00;
    vec4 vVec = p10 - p00;
    vec4 normal = normalize( vec4(cross(vVec.xyz, uVec.xyz), 0) );

    vec4 p0 = (p01 - p00) * u + p00;
    vec4 p1 = (p11 - p10) * u + p10;

    te_out.HeightCoord = ((p1-p0)*v + p0).xz;

    float height = perlin(((p1-p0)*v + p0).xz);
    vec4 p = (p1 - p0) * v + p0 + normal * height;

    gl_Position = projection * view * u_Model * p;
    te_out.FragPos = vec3(u_Model * p);
    te_out.FragPosLightSpace = lightSpaceMatrix * vec4(te_out.FragPos, 1.0);
}