#version 410 core
layout(quads, fractional_odd_spacing, ccw) in;

uniform sampler2D u_HeightMap;

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
	vec2 HeightmapCoord;
	vec2 TextureCoord;
    vec3 WorldPos;
} te_in[];

out TE_OUT
{
	vec3 FragPos;
    vec2 TexCoord;
	vec2 HeightCoord;
	vec3 SurfaceNormal;
//	vec3 SurfaceTangent;
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
	//float v1 = Random2D(randomInput);
	//float v2 = Random2D(randomInput + vec2(1.0, 0.0));
	//float v3 = Random2D(randomInput + vec2(0.0, 1.0));
	//float v4 = Random2D(randomInput + vec2(1.0, 1.0));

	vec2 w = vec2(fractional_X, fractional_Y);
	w = w*w*w*(10.0 + w*(-15.0 + 6.0*w));

	//fractional_X = smoothstep(0.0, 1.0, fractional_X);
	//fractional_Y = smoothstep(0.0, 1.0, fractional_Y);
	//return a + fractional_X*(b-a) + fractional_Y*c + fractional_X*fractional_Y*(d-c) - a*fractional_Y - fractional_X*fractional_Y*(b-a);
	float k0 = a, 
	k1 = b - a, 
	k2 = c - a, 
	k3 = d - c - b + a;

	return k0 + k1*w.x + k2*w.y + k3*w.x*w.y;
}

float perlin(vec2 st){
	float gDispFactor = 20.0;
    int octaves = 10;
    float power = 3.0;
    float freq = 0.01;
    const mat2 m = mat2(0.8,-0.6,0.6,0.8);
    
	float persistence = 0.5;
	float total = 0.0,
		frequency = 0.05*freq,
		amplitude = gDispFactor;
	for (int i = 0; i < octaves; ++i) {
		frequency *= 2.0;
		amplitude *= persistence;

        //st = frequency*m*st;

		vec2 v = frequency*m*(st + 10.0f);

		total += InterpolatedNoise(0, v.x, v.y) * amplitude;
	}

	return pow(total, power);
}

vec4 CalcPosition(float u, float v)
{
    vec2 h00 = te_in[0].HeightmapCoord;
    vec2 h01 = te_in[1].HeightmapCoord;
    vec2 h10 = te_in[2].HeightmapCoord;
    vec2 h11 = te_in[3].HeightmapCoord;

    vec2 h0 = (h01 - h00) * u + h00;
    vec2 h1 = (h11 - h10) * u + h10;
    vec2 hCoord = (h1 - h0) * v + h0;

    vec4 p00 = gl_in[0].gl_Position;
    vec4 p01 = gl_in[1].gl_Position;
    vec4 p10 = gl_in[2].gl_Position;
    vec4 p11 = gl_in[3].gl_Position;

    vec4 uVec = p01 - p00;
    vec4 vVec = p10 - p00;
    vec4 normal = normalize( vec4(cross(vVec.xyz, uVec.xyz), 0) );

//    float height = texture(u_HeightMap, hCoord).y * 64.0 - 16.0;

    vec2 t00 = te_in[0].WorldPos.xz;
    vec2 t01 = te_in[1].WorldPos.xz;
    vec2 t10 = te_in[2].WorldPos.xz;
    vec2 t11 = te_in[3].WorldPos.xz;

    vec2 t0 = (t01 - t00) * u + t00;
    vec2 t1 = (t11 - t10) * u + t10;
    vec2 tCoord = (t1 - t0) * v + t0;


    vec4 p0 = (p01 - p00) * u + p00;
    vec4 p1 = (p11 - p10) * u + p10;

    float height = perlin(((p1-p0)*v + p0).xz);

    return (p1 - p0) * v + p0 + normal * height;
}

void main()
{
    float u = gl_TessCoord.x;
    float v = gl_TessCoord.y;

    vec2 h00 = te_in[0].HeightmapCoord;
    vec2 h01 = te_in[1].HeightmapCoord;
    vec2 h10 = te_in[2].HeightmapCoord;
    vec2 h11 = te_in[3].HeightmapCoord;

    vec2 h0 = (h01 - h00) * u + h00;
    vec2 h1 = (h11 - h10) * u + h10;
    vec2 hCoord = (h1 - h0) * v + h0;

//    float height = perlin(hCoord);
//    float height = texture(u_HeightMap, hCoord).r * 64.0 - 16.0;
    te_out.HeightCoord = hCoord;

    vec2 t00 = te_in[0].TextureCoord;
    vec2 t01 = te_in[1].TextureCoord;
    vec2 t10 = te_in[2].TextureCoord;
    vec2 t11 = te_in[3].TextureCoord;

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

    vec4 pCenter = CalcPosition(u, v);
    vec4 p0 = CalcPosition(u - u_TexelSizeU, v);
    vec4 p1 = CalcPosition(u + u_TexelSizeU, v);
    vec4 p2 = CalcPosition(u, v - u_TexelSizeV);
    vec4 p3 = CalcPosition(u, v + u_TexelSizeV);

    vec3 n0 = normalize(-cross(vec3(pCenter - p0), vec3(p3 - pCenter)));
    vec3 n1 = normalize(-cross(vec3(p1 - pCenter), vec3(p3 - pCenter)));
    vec3 n2 = normalize(-cross(vec3(pCenter - p2), vec3(p0 - pCenter)));
    vec3 n3 = normalize(-cross(vec3(p2 - pCenter), vec3(p1 - pCenter)));

    te_out.SurfaceNormal = normalize(transpose(inverse(mat3(u_Model))) * (n0 + n1 + n2 + n3));
//    vec3 tangent = normalize(mat3(u_Model) * vec3(p1 - p0));
//    vec3 bitangent = normalize(cross(te_out.SurfaceNormal, tangent));
//    te_out.SurfaceTangent = normalize(cross(bitangent, te_out.SurfaceNormal));
//
    gl_Position = projection * view * u_Model * pCenter;
    te_out.FragPos = vec3(u_Model * pCenter);
    te_out.FragPosLightSpace = lightSpaceMatrix * vec4(te_out.FragPos, 1.0);
}