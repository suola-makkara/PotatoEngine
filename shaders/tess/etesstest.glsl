#version 460 core

layout (triangles, fractional_odd_spacing, ccw) in;

out vec3 normal;
out vec4 pos;
out vec3 color;

uniform mat4 uView;
uniform mat4 uProj;
uniform vec3 uCameraPos;
uniform vec2 uChunkPos;
uniform float uChunkSize;

uniform sampler2D uNoiseTex;
uniform int uBaseLod;
uniform int uMaxLod;
//uniform sampler2D uNoiseNormalTex;

//uniform float uTextureFreq;
//uniform float uHeightScale;

void main()
{
	pos = gl_TessCoord.x * gl_in[0].gl_Position
		+ gl_TessCoord.y * gl_in[1].gl_Position
		+ gl_TessCoord.z * gl_in[2].gl_Position;

	float lod = max(0, log2(length(pos.xyz - uCameraPos) + 1) - 5);
	lod = min(uMaxLod, max(uBaseLod, lod));

	int lod0 = int(floor(lod));
	int lod1 = int(ceil(lod));

	vec2 texSize0 = vec2(textureSize(uNoiseTex, lod0 - uBaseLod));
	vec2 texSize1 = vec2(textureSize(uNoiseTex, lod1 - uBaseLod));

	vec2 texCoord0 = ((texSize0 - 2.0) * vec2(pos.x - uChunkPos.x, pos.z - uChunkPos.y) / uChunkSize + 1.0) / texSize0;
	vec2 texCoord1 = ((texSize1 - 2.0) * vec2(pos.x - uChunkPos.x, pos.z - uChunkPos.y) / uChunkSize + 1.0) / texSize1;

	float h0 = textureLod(uNoiseTex, texCoord0, lod0 - uBaseLod).x * 5.0;
	float h1 = textureLod(uNoiseTex, texCoord1, lod1 - uBaseLod).x * 5.0;

	pos.y = mix(h0, h1, lod - floor(lod));
	
	gl_Position = uProj * uView * pos;
}