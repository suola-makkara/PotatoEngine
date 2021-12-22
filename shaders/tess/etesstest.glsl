#version 460 core

layout (triangles, fractional_odd_spacing, ccw) in;

out vec3 normal;
out vec4 pos;

uniform mat4 uView;
uniform mat4 uProj;

uniform sampler2D uNoiseTex;
uniform sampler2D uNoiseNormalTex;

uniform float uTextureFreq;
uniform float uHeightScale;

void main()
{
	pos = gl_TessCoord.x * gl_in[0].gl_Position
		+ gl_TessCoord.y * gl_in[1].gl_Position
		+ gl_TessCoord.z * gl_in[2].gl_Position;

	vec2 texCoord = vec2(pos.x * uTextureFreq, pos.z * uTextureFreq);

	pos.y = texture(uNoiseTex, texCoord).x * uHeightScale;
	
	// sin(pos.x) + sin(pos.z) + sin(3 * pos.x) + sin(3 * pos.z);

	normal = normalize(texture(uNoiseNormalTex, texCoord).xyz - 0.5);
	
	//normalize(cross(vec3(0, cos(pos.z) + 3 * cos(3 * pos.x), 1), vec3(1, cos(pos.x) + 3 * cos(3 * pos.z), 0)));

	gl_Position = uProj * uView * pos;
}