#version 410 core

layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 BrightColor;

uniform sampler2D u_Heightmap;
uniform sampler2D u_Texture;
uniform sampler2D u_NormalTexture;
uniform sampler2D u_MountainTexture;
uniform sampler2D u_MountainNormalTexture;
uniform int u_TextureMethodType = 0;
uniform mat4 u_Model;
uniform float u_TexelSizeU;
uniform float u_TexelSizeV;
uniform vec3 u_LightPos;
uniform vec3 u_ViewPos;
uniform vec3 u_FogColor;

layout (std140) uniform u_DirLight
{
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

in vec2 TexCoord;
in vec2 HeightCoord;
in vec3 FragPos;

float sum( vec3 v ) { return v.x+v.y+v.z; }

vec4 hash4( vec2 p ) { return fract(sin(vec4( 1.0+dot(p,vec2(37.0,17.0)), 
                                              2.0+dot(p,vec2(11.0,47.0)),
                                              3.0+dot(p,vec2(41.0,29.0)),
                                              4.0+dot(p,vec2(23.0,31.0))))*103.0); }

vec4 textureRotation(sampler2D samp, vec2 uv)
{
    vec2 iuv = floor( uv );
    vec2 fuv = fract( uv );

    // generate per-tile transform
    vec4 ofa = hash4( iuv + vec2(0.0,0.0) );
    vec4 ofb = hash4( iuv + vec2(1.0,0.0) );
    vec4 ofc = hash4( iuv + vec2(0.0,1.0) );
    vec4 ofd = hash4( iuv + vec2(1.0,1.0) );
    
    vec2 ddx = dFdx( uv );
    vec2 ddy = dFdy( uv );

    // transform per-tile uvs
    ofa.zw = sign(ofa.zw-0.5);
    ofb.zw = sign(ofb.zw-0.5);
    ofc.zw = sign(ofc.zw-0.5);
    ofd.zw = sign(ofd.zw-0.5);
    
    // uv's, and derivarives (for correct mipmapping)
    vec2 uva = uv*ofa.zw + ofa.xy; vec2 ddxa = ddx*ofa.zw; vec2 ddya = ddy*ofa.zw;
    vec2 uvb = uv*ofb.zw + ofb.xy; vec2 ddxb = ddx*ofb.zw; vec2 ddyb = ddy*ofb.zw;
    vec2 uvc = uv*ofc.zw + ofc.xy; vec2 ddxc = ddx*ofc.zw; vec2 ddyc = ddy*ofc.zw;
    vec2 uvd = uv*ofd.zw + ofd.xy; vec2 ddxd = ddx*ofd.zw; vec2 ddyd = ddy*ofd.zw;

    // fetch and blend
    vec2 b = smoothstep(0.25,0.75,fuv);
    
//    return textureGrad(samp, uva, ddxa, ddya);
    return mix( mix( textureGrad( samp, uva, ddxa, ddya ), 
                     textureGrad( samp, uvb, ddxb, ddyb ), b.x ), 
                mix( textureGrad( samp, uvc, ddxc, ddyc ),
                     textureGrad( samp, uvd, ddxd, ddyd ), b.x), b.y );
}

vec4 textureVoronoi( sampler2D samp, vec2 uv )
{
    vec2 p = floor( uv );
    vec2 f = fract( uv );
	
    // derivatives (for correct mipmapping)
    vec2 ddx = dFdx( uv );
    vec2 ddy = dFdy( uv );
    
	vec3 va = vec3(0.0);
	float w1 = 0.0;
    float w2 = 0.0;
    for( int j=-1; j<=1; j++ )
    for( int i=-1; i<=1; i++ )
    {
        vec2 g = vec2( float(i),float(j) );
		vec4 o = hash4( p + g );
		vec2 r = g - f + o.xy;
		float d = dot(r,r);
        float w = exp(-5.0*d );
        vec3 c = textureGrad( samp, uv + o.zw, ddx, ddy ).xyz;
		va += w*c;
		w1 += w;
        w2 += w*w;
    }
    
    // normal averaging --> lowers contrasts
    return vec4(va/w1, 1.0);

    // contrast preserving average
    float mean = 0.3;// textureGrad( samp, uv, ddx*16.0, ddy*16.0 ).x;
    return vec4((mean + (va-w1*mean)/sqrt(w2)), 1.0);
}

vec4 textureOffset( sampler2D samp, vec2 x )
{
	float k = hash4(0.0005*x).x;
    //float k = texture( iChannel1, 0.005*x ).x; // cheap (cache friendly) lookup
    
    vec2 duvdx = dFdx( x );
    vec2 duvdy = dFdy( x );
    
    float l = k*8.0;
    float f = fract(l);
    
    float ia = floor(l);
    float ib = ia + 1.0;
    
    vec2 offa = sin(vec2(3.0,7.0)*ia); // can replace with any other hash
    vec2 offb = sin(vec2(3.0,7.0)*ib); // can replace with any other hash

    vec3 cola = textureGrad( samp, x + offa, duvdx, duvdy ).xyz;
    vec3 colb = textureGrad( samp, x + offb, duvdx, duvdy ).xyz;
    
    return vec4(mix( cola, colb, smoothstep(0.2,0.8,f-0.1*sum(cola-colb)) ), 1.0);
}

mat3 CalcTBN()
{
    float h = texture(u_Heightmap, HeightCoord).y * 64.0 - 16.0;
    float left = texture(u_Heightmap, HeightCoord + vec2(-u_TexelSizeU, 0.0)).y * 64.0 - 16.0;
    float right = texture(u_Heightmap, HeightCoord + vec2(u_TexelSizeU, 0.0)).y * 64.0 - 16.0;
    float up = texture(u_Heightmap, HeightCoord + vec2(0.0, u_TexelSizeV)).y * 64.0 - 16.0;
    float down = texture(u_Heightmap, HeightCoord + vec2(0.0, -u_TexelSizeV)).y * 64.0 - 16.0;

    vec3 n0 = normalize(cross(vec3(1.0, h-left, 0), vec3(0, up-h, 1.0)));
    vec3 n1 = normalize(cross(vec3(1.0, right-h, 0), vec3(0, up-h, 1.0)));
    vec3 n2 = normalize(cross(vec3(0, h-down, 1.0), vec3(-1.0, left-h, 0)));
    vec3 n3 = normalize(cross(vec3(1.0, down-h,-1.0), vec3(1.0, right-h, 0)));

    mat3 model = mat3(u_Model);
    // TODO: why minus?
    vec3 normal = -normalize(model * (n0 + n1 + n2 + n3));

    vec3 tangent = normalize(model * vec3(2.0, right - left, 0.0));
    vec3 bitangent = normalize(cross(normal, tangent));
    tangent = normalize(cross(bitangent, normal));
    mat3 TBN = mat3(tangent, bitangent, normal);
    return TBN;
}

sampler2D GetTextureSampler(float height, float threshold)
{
    if (height > threshold)
        return u_MountainTexture;
    return u_Texture;
}

sampler2D GetNormalTextureSampler(float height, float threshold)
{
    if (height > threshold)
        return u_MountainNormalTexture;
    return u_NormalTexture;
}

vec4 GetTexture(sampler2D sampler)
{
    switch(u_TextureMethodType)
    {
    case 0:
		return textureRotation(sampler, TexCoord);
		break;
    case 1:
		return textureVoronoi(sampler, TexCoord);
		break;
    case 2:
		return textureOffset(sampler, TexCoord);
    }
}

void main()
{
	vec4 texColor;
    vec3 normalMap;

    float height = texture(u_Heightmap, HeightCoord).y * 64.0 - 16.0;

    vec4 lowTexture = GetTexture(u_Texture);
    vec4 highTexture = GetTexture(u_MountainTexture);
    vec3 lowNormal = vec3(GetTexture(u_NormalTexture));
    vec3 highNormal = vec3(GetTexture(u_MountainNormalTexture));

    texColor = mix(lowTexture, highTexture, smoothstep(-3.0, 3.0, height - 15.0));
    normalMap = mix(lowNormal, highNormal, smoothstep(-3.0, 3.0, height - 15.0));

//    switch(u_TextureMethodType)
//    {
//    case 0:
//		texColor = textureRotation(GetTextureSampler(height, 15.0), TexCoord);
//        normalMap = textureRotation(GetNormalTextureSampler(height, 15.0), TexCoord).rgb;
//		break;
//    case 1:
//		texColor = textureVoronoi(GetTextureSampler(height, 15.0), TexCoord);
//        normalMap = textureVoronoi(GetNormalTextureSampler(height, 15.0), TexCoord).rgb;
//		break;
//    case 2:
//		texColor = textureOffset(GetTextureSampler(height, 15.0), TexCoord);
//        normalMap = textureOffset(GetNormalTextureSampler(height, 15.0), TexCoord).rgb;
//    }
//
//
	float ambientStrength = 0.1f;
	vec3 ambientColor = ambientStrength * ambient;

    mat3 TBN = CalcTBN();
	normalMap = normalMap * 2.0 - 1.0;
    vec3 normal = normalize(TBN * normalMap);

    vec3 lightDir = normalize(u_LightPos - FragPos);
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuseColor = diffuse * diff * vec3(texColor);

	float fogStart = 10.0f;
	float fogEnd = 500.0f;
	vec4 fogColor = vec4(u_FogColor, 1.0);
	float distance = length(FragPos - u_ViewPos);
	float visibility = 1.0;
	visibility = max(min((fogEnd - distance) / (fogEnd - fogStart), 1.0), 0.0);

    vec4 color = vec4(ambientColor, 1.0) + vec4(diffuseColor, 1.0);

	vec4 foggedColor = mix(fogColor, color, visibility);

    FragColor = foggedColor;
    BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
    
	if (distance > 500.0)
		discard;
}
