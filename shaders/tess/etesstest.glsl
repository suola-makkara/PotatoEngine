#version 460 core

layout (triangles, fractional_odd_spacing, ccw) in;

out vec3 normal;
out vec4 pos;
out vec3 color;

uniform mat4 uView;
uniform mat4 uProj;
uniform vec3 uCameraPos;

uniform sampler2D uNoiseTex;
uniform sampler2D uNoiseNormalTex;

uniform float uTextureFreq;
uniform float uHeightScale;

void main()
{
	pos = gl_TessCoord.x * gl_in[0].gl_Position
		+ gl_TessCoord.y * gl_in[1].gl_Position
		+ gl_TessCoord.z * gl_in[2].gl_Position;

	float lod = max(0, log2(length(pos.xyz - uCameraPos) + 1) - 3);

	vec2 texCoord = vec2(pos.x * uTextureFreq, pos.z * uTextureFreq);

	pos.y = textureLod(uNoiseTex, texCoord, lod).x * uHeightScale;
//	float dydx = textureLod(uNoiseTex, texCoord + vec2(uTextureFreq * 0.01, 0.0), 0).x * uHeightScale - pos.y;
//	float dydz = textureLod(uNoiseTex, texCoord + vec2(0.0, uTextureFreq * 0.01), 0).x * uHeightScale - pos.y;
//	normal = normalize(cross(vec3(0.0, dydz, 0.01), vec3(0.01, dydx, 0.0)));
	
	normal = normalize(textureLod(uNoiseNormalTex, texCoord, lod).xyz - 0.5);
	
	gl_Position = uProj * uView * pos;
}