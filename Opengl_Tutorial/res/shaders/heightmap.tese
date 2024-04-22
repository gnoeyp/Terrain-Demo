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

in vec2 HeightmapCoord[];
in vec2 TextureCoord[];

out vec2 TexCoord;
out vec3 FragPos;
out vec2 HeightCoord;
out vec3 SurfaceNormal;
out vec3 SurfaceTangent;
out vec4 FragPosLightSpace;

vec4 CalcPosition(float u, float v)
{
    vec2 h00 = HeightmapCoord[0];
    vec2 h01 = HeightmapCoord[1];
    vec2 h10 = HeightmapCoord[2];
    vec2 h11 = HeightmapCoord[3];

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

    float height = texture(u_HeightMap, hCoord).y * 64.0 - 16.0;

    vec4 p0 = (p01 - p00) * u + p00;
    vec4 p1 = (p11 - p10) * u + p10;
    return (p1 - p0) * v + p0 + normal * height;
}

void main()
{
    float u = gl_TessCoord.x;
    float v = gl_TessCoord.y;

    vec2 h00 = HeightmapCoord[0];
    vec2 h01 = HeightmapCoord[1];
    vec2 h10 = HeightmapCoord[2];
    vec2 h11 = HeightmapCoord[3];

    vec2 h0 = (h01 - h00) * u + h00;
    vec2 h1 = (h11 - h10) * u + h10;
    vec2 hCoord = (h1 - h0) * v + h0;

    float height = texture(u_HeightMap, hCoord).y * 64.0 - 16.0;
    HeightCoord = hCoord;

    vec2 t00 = TextureCoord[0];
    vec2 t01 = TextureCoord[1];
    vec2 t10 = TextureCoord[2];
    vec2 t11 = TextureCoord[3];

    vec2 t0 = (t01 - t00) * u + t00;
    vec2 t1 = (t11 - t10) * u + t10;
    TexCoord = (t1 - t0) * v + t0;

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

    SurfaceNormal = normalize(mat3(u_Model) * (n0 + n1 + n2 + n3));
    vec3 tangent = normalize(mat3(u_Model) * vec3(p1 - p0));
    vec3 bitangent = normalize(cross(SurfaceNormal, tangent));
    SurfaceTangent = normalize(cross(bitangent, SurfaceNormal));

    gl_Position = projection * view * u_Model * pCenter;
    FragPos = vec3(u_Model * pCenter);
    FragPosLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0);
}